#!/usr/bin/env ruby
# frozen_string_literal: true

require "xcodeproj"
require "fileutils"
require "rexml/document"

ROOT_DIR = File.expand_path("../..", __dir__)
IOS_DIR  = File.join(ROOT_DIR, "ios")

DEFAULT_XCODEPROJ = File.join(IOS_DIR, "JunoNativeApp.xcodeproj")
xcodeproj_path = ENV["XCODEPROJ_PATH"].to_s.strip
xcodeproj_path = DEFAULT_XCODEPROJ if xcodeproj_path.empty?

scheme_name = ENV["SCHEME_NAME"].to_s.strip
scheme_name = "JunoNative" if scheme_name.empty?

unless File.exist?(File.join(xcodeproj_path, "project.pbxproj"))
  abort "❌ Xcode project not found at: #{xcodeproj_path}"
end

project = Xcodeproj::Project.open(xcodeproj_path)
target  = project.targets.find { |t| t.name == scheme_name }

if target.nil?
  abort "❌ Target '#{scheme_name}' not found in #{xcodeproj_path}. Targets: #{project.targets.map(&:name).join(', ')}"
end

shared_dir = File.join(xcodeproj_path, "xcshareddata", "xcschemes")
FileUtils.mkdir_p(shared_dir)
scheme_path = File.join(shared_dir, "#{scheme_name}.xcscheme")

# Minimal, CI-friendly scheme that supports building, testing (if present), profiling, archiving.
# We intentionally generate XML directly instead of calling Xcodeproj's scheme helpers, because
# those APIs can differ across xcodeproj gem versions.
doc = REXML::Document.new
doc << REXML::XMLDecl.new("1.0", "UTF-8")

scheme = doc.add_element("Scheme", {
  "LastUpgradeVersion" => "9999",
  "version" => "1.7"
})

build_action = scheme.add_element("BuildAction", {
  "parallelizeBuildables" => "YES",
  "buildImplicitDependencies" => "YES"
})

build_entries = build_action.add_element("BuildActionEntries")
entry = build_entries.add_element("BuildActionEntry", {
  "buildForTesting" => "YES",
  "buildForRunning" => "YES",
  "buildForProfiling" => "YES",
  "buildForArchiving" => "YES",
  "buildForAnalyzing" => "YES"
})

entry.add_element("BuildableReference", {
  "BuildableIdentifier" => "primary",
  "BlueprintIdentifier" => target.uuid,
  "BuildableName" => target.product_reference&.path || "#{scheme_name}.app",
  "BlueprintName" => target.name,
  "ReferencedContainer" => "container:#{File.basename(xcodeproj_path)}"
})

test_action = scheme.add_element("TestAction", {
  "buildConfiguration" => "Debug",
  "selectedDebuggerIdentifier" => "Xcode.DebuggerFoundation.Debugger.LLDB",
  "selectedLauncherIdentifier" => "Xcode.DebuggerFoundation.Launcher.LLDB",
  "shouldUseLaunchSchemeArgsEnv" => "YES"
})
test_action.add_element("Testables")

scheme.add_element("LaunchAction", {
  "buildConfiguration" => "Debug",
  "selectedDebuggerIdentifier" => "Xcode.DebuggerFoundation.Debugger.LLDB",
  "selectedLauncherIdentifier" => "Xcode.DebuggerFoundation.Launcher.LLDB",
  "launchStyle" => "0",
  "useCustomWorkingDirectory" => "NO",
  "ignoresPersistentStateOnLaunch" => "NO",
  "debugDocumentVersioning" => "YES",
  "debugServiceExtension" => "internal",
  "allowLocationSimulation" => "YES"
})

scheme.add_element("ProfileAction", {
  "buildConfiguration" => "Release",
  "shouldUseLaunchSchemeArgsEnv" => "YES",
  "savedToolIdentifier" => "",
  "useCustomWorkingDirectory" => "NO",
  "debugDocumentVersioning" => "YES"
})

scheme.add_element("AnalyzeAction", { "buildConfiguration" => "Debug" })
scheme.add_element("ArchiveAction", { "buildConfiguration" => "Release", "revealArchiveInOrganizer" => "YES" })

formatter = REXML::Formatters::Pretty.new(2)
formatter.compact = true

File.open(scheme_path, "w") do |f|
  formatter.write(doc, f)
  f.write("\n")
end

puts "✅ Ensured shared scheme '#{scheme_name}' at: #{scheme_path}"
