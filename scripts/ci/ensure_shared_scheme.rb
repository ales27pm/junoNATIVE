#!/usr/bin/env ruby
# frozen_string_literal: true

require "xcodeproj"
require "fileutils"

ROOT_DIR = File.expand_path("../..", __dir__)
IOS_PROJ = File.join(ROOT_DIR, "ios", "JunoNative.xcodeproj")

scheme_name = ENV["IOS_SCHEME"].to_s
scheme_name = "JunoNative" if scheme_name.empty?

preferred_target = ENV["IOS_APP_TARGET"].to_s
preferred_target = "JunoNative" if preferred_target.empty?

unless File.exist?(IOS_PROJ)
  warn "❌ Xcode project not found: #{IOS_PROJ}"
  exit 1
end

project = Xcodeproj::Project.open(IOS_PROJ)

target = project.targets.find { |t| t.isa == "PBXNativeTarget" && t.name == preferred_target }
if target.nil?
  warn "❌ Could not find target '#{preferred_target}' in #{IOS_PROJ}"
  warn "   Targets: #{project.targets.map(&:name).join(', ')}"
  exit 1
end

puts "✅ Ensuring shared scheme '#{scheme_name}' builds target '#{target.name}'"

# Always generate a fresh scheme to avoid nil entries quirks across xcodeproj versions.
scheme = Xcodeproj::XCScheme.new

# Add the app target to the build action (this creates build_action + entries safely)
scheme.add_build_target(target)

# Ensure archive action uses Release (what xcodebuild archive expects)
scheme.archive_action ||= Xcodeproj::XCScheme::ArchiveAction.new(nil)
scheme.archive_action.build_configuration = "Release"

# Keep these sane (not strictly required for CI export, but harmless)
scheme.launch_action ||= Xcodeproj::XCScheme::LaunchAction.new(nil)
scheme.launch_action.build_configuration = "Release"
scheme.launch_action.runnable = Xcodeproj::XCScheme::BuildableProductRunnable.new(target)

scheme.profile_action ||= Xcodeproj::XCScheme::ProfileAction.new(nil)
scheme.profile_action.build_configuration = "Release"
scheme.profile_action.runnable = Xcodeproj::XCScheme::BuildableProductRunnable.new(target)

scheme.analyze_action ||= Xcodeproj::XCScheme::AnalyzeAction.new(nil)
scheme.analyze_action.build_configuration = "Release"

scheme.test_action ||= Xcodeproj::XCScheme::TestAction.new(nil)
scheme.test_action.build_configuration = "Release"

# Remove any old shared scheme file so we definitely overwrite
scheme_dir  = File.join(IOS_PROJ, "xcshareddata", "xcschemes")
scheme_path = File.join(scheme_dir, "#{scheme_name}.xcscheme")
FileUtils.mkdir_p(scheme_dir)
FileUtils.rm_f(scheme_path)

# Save as a SHARED scheme
scheme.save_as(IOS_PROJ, scheme_name, true)

puts "✅ Shared scheme written: #{scheme_path}"
