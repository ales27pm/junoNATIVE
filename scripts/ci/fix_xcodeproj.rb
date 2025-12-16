#!/usr/bin/env ruby
# frozen_string_literal: true

require "xcodeproj"
require "fileutils"

ROOT_DIR = File.expand_path("../..", __dir__)
IOS_PROJ = File.join(ROOT_DIR, "ios", "JunoNative.xcodeproj")

expected_bundle = ENV["APP_IDENTIFIER"].to_s
expected_team   = ENV["APPLE_TEAM_ID"].to_s
preferred_name  = ENV["IOS_APP_TARGET"].to_s # optional override

unless File.exist?(IOS_PROJ)
  warn "❌ Xcode project not found: #{IOS_PROJ}"
  exit 1
end

project = Xcodeproj::Project.open(IOS_PROJ)

def app_like_target?(t, expected_bundle)
  return false unless t.isa == "PBXNativeTarget"

  cfgs = t.build_configurations
  return false if cfgs.nil? || cfgs.empty?

  settings = cfgs.map(&:build_settings)

  # Strong signals for an iOS .app:
  wrapper_app = settings.any? { |s| s["WRAPPER_EXTENSION"].to_s == "app" }
  product_type_app = t.respond_to?(:product_type) && t.product_type.to_s.include?("application")
  sdk_ios = settings.any? do |s|
    s["SDKROOT"].to_s.include?("iphoneos") ||
      s["SDKROOT"].to_s.include?("iphone") ||
      s["PLATFORM_NAME"].to_s == "iphoneos"
  end

  bundle_ok =
    expected_bundle.to_s.empty? ||
    settings.any? { |s| s["PRODUCT_BUNDLE_IDENTIFIER"].to_s == expected_bundle } ||
    # if bundle id is set via config vars, don't block detection
    settings.any? { |s| s["PRODUCT_BUNDLE_IDENTIFIER"].to_s.include?("$(") }

  # Many RN app targets don’t explicitly set SDKROOT; rely on wrapper/product_type too.
  (wrapper_app || product_type_app) && bundle_ok && (sdk_ios || wrapper_app || product_type_app)
end

candidates = project.targets.select { |t| app_like_target?(t, expected_bundle) }

# If detection is too strict, fallback: look for target matching name “JunoNative”
if candidates.empty?
  candidates = project.targets.select { |t| t.isa == "PBXNativeTarget" && t.name.to_s == "JunoNative" }
end

if !preferred_name.empty?
  preferred = candidates.find { |t| t.name == preferred_name }
  candidates = [preferred].compact if preferred
end

if candidates.empty?
  warn "❌ No iOS application targets found in #{IOS_PROJ}."
  warn "   Targets seen: #{project.targets.map(&:name).join(', ')}"
  exit 1
end

target = candidates.first
puts "✅ Using target: #{target.name}"

changed = false

target.build_configurations.each do |cfg|
  # Archive uses Release by default; patch Release + anything Release-like
  name = cfg.name.to_s
  next unless name.match?(/Release|Archive|AppStore|Production/i) || name == "Debug" # safe: Debug doesn't affect export

  bs = cfg.build_settings

  # The classic “generic archive / no .app” fix
  if bs["SKIP_INSTALL"].to_s != "NO"
    bs["SKIP_INSTALL"] = "NO"
    changed = true
    puts "  - #{cfg.name}: SKIP_INSTALL=NO"
  end

  if bs["INSTALL_PATH"].to_s.empty? || bs["INSTALL_PATH"].to_s == "$(LOCAL_APPS_DIR)"
    bs["INSTALL_PATH"] = "/Applications"
    changed = true
    puts "  - #{cfg.name}: INSTALL_PATH=/Applications"
  end

  # Optional: set team id if not already set (won’t override if present)
  if !expected_team.empty? && bs["DEVELOPMENT_TEAM"].to_s.empty?
    bs["DEVELOPMENT_TEAM"] = expected_team
    changed = true
    puts "  - #{cfg.name}: DEVELOPMENT_TEAM=#{expected_team}"
  end

  # Optional: align bundle id ONLY if it's a literal (don’t break variable-based setups)
  if !expected_bundle.empty?
    cur = bs["PRODUCT_BUNDLE_IDENTIFIER"].to_s
    if !cur.empty? && !cur.include?("$(") && cur != expected_bundle
      bs["PRODUCT_BUNDLE_IDENTIFIER"] = expected_bundle
      changed = true
      puts "  - #{cfg.name}: PRODUCT_BUNDLE_IDENTIFIER=#{expected_bundle}"
    end
  end
end

if changed
  project.save
  puts "✅ Project patched and saved."
else
  puts "ℹ️ No project changes required."
end
