# scripts/ci/fix_xcodeproj.rb
# Patches the iOS project so Archive includes a real .app (not a generic archive).

require "xcodeproj"
require "fileutils"

root = File.expand_path("../..", __dir__)
proj_path = File.join(root, "ios", "JunoNative.xcodeproj")
pbxproj = File.join(proj_path, "project.pbxproj")

unless File.exist?(pbxproj)
  warn "❌ Missing #{pbxproj}"
  exit 1
end

bundle_id = ENV["APP_IDENTIFIER"].to_s
target_name = ENV["IOS_APP_TARGET"].to_s
target_name = "JunoNative" if target_name.empty?

project = Xcodeproj::Project.open(proj_path)

targets = project.targets
if targets.empty?
  warn "❌ No targets found in #{proj_path}"
  exit 1
end

puts "🔎 Targets seen: #{targets.map(&:name).join(', ')}"

# Pick a target:
# 1) exact name match
# 2) bundle id match in build settings
candidates = targets.select { |t| t.name == target_name }

if candidates.empty? && !bundle_id.empty?
  candidates = targets.select do |t|
    t.build_configurations.any? do |cfg|
      cfg.build_settings["PRODUCT_BUNDLE_IDENTIFIER"].to_s == bundle_id
    end
  end
end

# Fall back: any PBXNativeTarget named like the app
if candidates.empty?
  candidates = targets.select { |t| t.is_a?(Xcodeproj::Project::Object::PBXNativeTarget) && t.name.downcase.include?("juno") }
end

if candidates.empty?
  warn "❌ No candidate targets found to patch in #{proj_path}."
  warn "   If this project truly has no iOS app target, you must regenerate the iOS project (it will always archive as generic)."
  exit 1
end

candidates.each do |t|
  puts "🛠  Patching target: #{t.name} (#{t.class})"

  # Some repos accidentally end up with a non-application product type.
  # If it smells like an app (has Info.plist + bundle id), force product_type.
  begin
    has_plist = t.build_configurations.any? { |cfg| cfg.build_settings["INFOPLIST_FILE"].to_s != "" }
    has_bundle = t.build_configurations.any? { |cfg| cfg.build_settings["PRODUCT_BUNDLE_IDENTIFIER"].to_s != "" }

    if t.respond_to?(:product_type) && (t.product_type.to_s.empty? || (!t.product_type.to_s.include?("application") && has_plist && has_bundle))
      puts "  • Setting product_type => com.apple.product-type.application (was: #{t.product_type})"
      t.product_type = "com.apple.product-type.application"
    end
  rescue => e
    warn "  ⚠️ Could not adjust product_type for #{t.name}: #{e}"
  end

  t.build_configurations.each do |cfg|
    name = cfg.name.to_s
    bs = cfg.build_settings

    # IMPORTANT: apps must be SKIP_INSTALL=NO for Archive, otherwise Products/Applications is missing.
    if name.downcase.include?("release") || name.downcase.include?("archive")
      puts "  • #{name}: SKIP_INSTALL => NO"
      bs["SKIP_INSTALL"] = "NO"
      bs["INSTALL_PATH"] = "/Applications"
    end

    # Helps CI consistency
    bs["ONLY_ACTIVE_ARCH"] = "NO" if bs["ONLY_ACTIVE_ARCH"].to_s.empty?
    bs["DEAD_CODE_STRIPPING"] = "YES" if bs["DEAD_CODE_STRIPPING"].to_s.empty?
  end
end

project.save
puts "✅ Saved patched project: #{proj_path}"
