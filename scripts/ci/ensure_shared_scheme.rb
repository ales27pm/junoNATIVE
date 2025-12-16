#!/usr/bin/env ruby
# ------------------------------------------------------------
# ensure_shared_scheme.rb
#
# Safely ensures a shared scheme exists and builds the app
# WITHOUT touching nil build / launch actions.
#
# CI-safe, idempotent, Xcode-version-tolerant.
# ------------------------------------------------------------

require "xcodeproj"
require "fileutils"

PROJECT_PATH = "ios/JunoNative.xcodeproj"
SCHEME_NAME  = "JunoNative"

unless File.exist?(PROJECT_PATH)
  abort "❌ Xcode project not found at #{PROJECT_PATH}"
end

project = Xcodeproj::Project.open(PROJECT_PATH)

# ------------------------------------------------------------
# Locate application target
# ------------------------------------------------------------
target = project.targets.find do |t|
  t.product_type == "com.apple.product-type.application"
end

unless target
  abort "❌ No application target found. Did fix_ios_project.rb run?"
end

puts "✅ Found application target: #{target.name}"

# ------------------------------------------------------------
# Prepare scheme paths
# ------------------------------------------------------------
shared_dir = File.join(PROJECT_PATH, "xcshareddata", "xcschemes")
FileUtils.mkdir_p(shared_dir)

scheme_path = File.join(shared_dir, "#{SCHEME_NAME}.xcscheme")

# ------------------------------------------------------------
# Load or create scheme
# ------------------------------------------------------------
scheme =
  if File.exist?(scheme_path)
    Xcodeproj::XCScheme.new(scheme_path)
  else
    Xcodeproj::XCScheme.new
  end

# ------------------------------------------------------------
# Build Action (SAFE)
# ------------------------------------------------------------
build_action = scheme.build_action

unless build_action
  build_action = Xcodeproj::XCScheme::BuildAction.new
  scheme.build_action = build_action
end

entries = build_action.entries || []

# Remove stale entries referencing old static library
entries.reject! do |e|
  e.buildable_references.any? do |br|
    br.target_name != target.name
  end
end

# Add entry if missing
unless entries.any? { |e| e.buildable_references.any? { |br| br.target_name == target.name } }
  entries << Xcodeproj::XCScheme::BuildAction::Entry.new(
    target,
    true,  # build_for_running
    true,  # build_for_testing
    true,  # build_for_profiling
    true,  # build_for_archiving
    true   # build_for_analyzing
  )
end

build_action.entries = entries
scheme.build_action  = build_action

# ------------------------------------------------------------
# Launch Action (NO runnable=, NO crashes)
# ------------------------------------------------------------
launch_action = scheme.launch_action ||
                Xcodeproj::XCScheme::LaunchAction.new

# IMPORTANT:
# We DO NOT set `runnable=` because it is not supported
# consistently across Xcodeproj versions.
launch_action.build_configuration ||= "Release"
launch_action.selected_debugger_identifier ||= "Xcode.DebuggerFoundation.Debugger.LLDB"
launch_action.selected_launcher_identifier ||= "Xcode.DebuggerFoundation.Launcher.LLDB"
launch_action.launch_style ||= "0"
launch_action.ignores_persistent_state_on_launch ||= false
launch_action.debug_document_versioning ||= true

scheme.launch_action = launch_action

# ------------------------------------------------------------
# Archive Action
# ------------------------------------------------------------
archive_action = scheme.archive_action ||
                 Xcodeproj::XCScheme::ArchiveAction.new

archive_action.build_configuration ||= "Release"
archive_action.reveal_archive_in_organizer ||= true
scheme.archive_action = archive_action

# ------------------------------------------------------------
# Save
# ------------------------------------------------------------
scheme.save_as(PROJECT_PATH, SCHEME_NAME, true)

puts "✅ Shared scheme '#{SCHEME_NAME}' ensured successfully"
