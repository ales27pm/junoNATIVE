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

target = project.targets.find do |t|
  t.isa == "PBXNativeTarget" && t.name == preferred_target
end

if target.nil?
  warn "❌ Could not find target '#{preferred_target}' in #{IOS_PROJ}"
  warn "   Targets: #{project.targets.map(&:name).join(', ')}"
  exit 1
end

puts "✅ Ensuring shared scheme '#{scheme_name}' builds target '#{target.name}'"

# Generate a fresh scheme (avoids nil entries quirks and version differences).
scheme = Xcodeproj::XCScheme.new

# This is the critical part: ensure the APP target is in the scheme's BuildAction.
scheme.add_build_target(target)

# Ensure Archive builds Release.
scheme.archive_action ||= Xcodeproj::XCScheme::ArchiveAction.new(nil)
scheme.archive_action.build_configuration = "Release"

# Optional but harmless: keep analyze/test actions set to Release (no runnable setters needed).
scheme.analyze_action ||= Xcodeproj::XCScheme::AnalyzeAction.new(nil)
scheme.analyze_action.build_configuration = "Release"

scheme.test_action ||= Xcodeproj::XCScheme::TestAction.new(nil)
scheme.test_action.build_configuration = "Release"

# Write as a SHARED scheme (so CI sees it).
scheme_dir  = File.join(IOS_PROJ, "xcshareddata", "xcschemes")
scheme_path = File.join(scheme_dir, "#{scheme_name}.xcscheme")
FileUtils.mkdir_p(scheme_dir)
FileUtils.rm_f(scheme_path)

scheme.save_as(IOS_PROJ, scheme_name, true)

puts "✅ Shared scheme written: #{scheme_path}"
