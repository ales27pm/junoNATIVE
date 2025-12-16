#!/usr/bin/env ruby
# ------------------------------------------------------------
# fix_xcodeproj.rb
#
# Safely normalizes build settings for the iOS APPLICATION
# target after conversion from static library.
#
# - No scheme manipulation
# - No assumptions about phases
# - CI-safe
# ------------------------------------------------------------

require "xcodeproj"

PROJECT_PATH = "ios/JunoNative.xcodeproj"

unless File.exist?(PROJECT_PATH)
  abort "❌ Xcode project not found at #{PROJECT_PATH}"
end

project = Xcodeproj::Project.open(PROJECT_PATH)

# ------------------------------------------------------------
# Find application target
# ------------------------------------------------------------
target = project.targets.find do |t|
  t.product_type == "com.apple.product-type.application"
end

unless target
  abort "❌ No application target found. Did fix_ios_project.rb run?"
end

puts "✅ Fixing build settings for target: #{target.name}"

# ------------------------------------------------------------
# Normalize build settings
# ------------------------------------------------------------
target.build_configurations.each do |config|
  s = config.build_settings

  # --- Core App Settings ---
  s["SKIP_INSTALL"] = "NO"
  s["APPLICATION_EXTENSION_API_ONLY"] = "NO"

  # --- React Native / New Architecture ---
  s["CLANG_CXX_LANGUAGE_STANDARD"] = "c++17"
  s["CLANG_CXX_LIBRARY"] = "libc++"
  s["OTHER_CPLUSPLUSFLAGS"] ||= ["$(inherited)", "-std=c++17"]

  # --- Linking ---
  s["LD_RUNPATH_SEARCH_PATHS"] ||= ["$(inherited)", "@executable_path/Frameworks"]

  # --- Disable Bitcode (required for RN + C++ DSP) ---
  s["ENABLE_BITCODE"] = "NO"

  # --- Ensure Info.plist ---
  s["INFOPLIST_FILE"] ||= "JunoNative/Info.plist"

  # --- Avoid CI signing conflicts ---
  s["CODE_SIGN_STYLE"] ||= "Manual"
  s["CODE_SIGNING_ALLOWED"] ||= "NO"
  s["CODE_SIGNING_REQUIRED"] ||= "NO"

  # --- Swift compatibility ---
  s["SWIFT_VERSION"] ||= "5.0"

  # --- Header Search Paths (safe append) ---
  header_paths = s["HEADER_SEARCH_PATHS"] || ["$(inherited)"]
  header_paths = [header_paths] if header_paths.is_a?(String)

  header_paths << "$(SRCROOT)/../rtn-juno-engine/cpp"
  header_paths << "$(SRCROOT)/../rtn-juno-engine/cpp/engine"
  header_paths << "$(SRCROOT)/../rtn-juno-engine/cpp/dsp"

  s["HEADER_SEARCH_PATHS"] = header_paths.uniq

  # --- Framework Search Paths ---
  framework_paths = s["FRAMEWORK_SEARCH_PATHS"] || ["$(inherited)"]
  framework_paths = [framework_paths] if framework_paths.is_a?(String)
  s["FRAMEWORK_SEARCH_PATHS"] = framework_paths.uniq
end

# ------------------------------------------------------------
# Ensure "Bundle React Native code and images" phase exists
# ------------------------------------------------------------
bundle_phase_name = "Bundle React Native code and images"

bundle_phase =
  target.shell_script_build_phases.find { |p| p.name == bundle_phase_name }

unless bundle_phase
  bundle_phase = target.new_shell_script_build_phase(bundle_phase_name)
  bundle_phase.shell_script = <<~SCRIPT
    export NODE_BINARY=node
    ../node_modules/react-native/scripts/react-native-xcode.sh
  SCRIPT
  puts "➕ Added React Native bundle phase"
end

# ------------------------------------------------------------
# Save project
# ------------------------------------------------------------
project.save
puts "✅ Xcode project settings normalized successfully"
