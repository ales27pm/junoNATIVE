# scripts/ci/fix_xcodeproj.rb
# Patches the iOS project so Archive includes a real .app (not a generic archive / empty Applications folder).

require "xcodeproj"

root      = File.expand_path("../..", __dir__)
proj_path = File.join(root, "ios", "JunoNative.xcodeproj")
pbxproj   = File.join(proj_path, "project.pbxproj")

unless File.exist?(pbxproj)
  warn "❌ Missing #{pbxproj}"
  exit 1
end

bundle_id   = ENV["APP_IDENTIFIER"].to_s
target_name = ENV["IOS_APP_TARGET"].to_s
target_name = "JunoNative" if target_name.empty?

project = Xcodeproj::Project.open(proj_path)
targets = project.targets

if targets.empty?
  warn "❌ No targets found in #{proj_path}"
  exit 1
end

puts "🔎 Targets seen: #{targets.map(&:name).join(', ')}"

# Heuristic selection:
# 1) exact name match
# 2) bundle id match
# 3) fallback: anything that looks like the app (not Pods-*)
candidates = targets.select { |t| t.name == target_name }

if candidates.empty? && !bundle_id.empty?
  candidates = targets.select do |t|
    t.build_configurations.any? do |cfg|
      cfg.build_settings["PRODUCT_BUNDLE_IDENTIFIER"].to_s == bundle_id
    end
  end
end

if candidates.empty?
  candidates = targets.select do |t|
    t.is_a?(Xcodeproj::Project::Object::PBXNativeTarget) &&
      !t.name.start_with?("Pods-") &&
      t.name.downcase.include?("juno")
  end
end

if candidates.empty?
  warn "❌ No candidate targets found to patch in #{proj_path}."
  warn "   If this project truly has no iOS app target, it will always archive as generic."
  exit 1
end

def patch_cfg!(cfg, bundle_id:)
  name = cfg.name.to_s
  bs   = cfg.build_settings

  # If bundle id is missing, set it (helps when project is half-generated)
  if !bundle_id.to_s.empty? && bs["PRODUCT_BUNDLE_IDENTIFIER"].to_s.strip.empty?
    puts "  • #{name}: PRODUCT_BUNDLE_IDENTIFIER => #{bundle_id}"
    bs["PRODUCT_BUNDLE_IDENTIFIER"] = bundle_id
  end

  # The big one: if SKIP_INSTALL=YES, archive Applications folder can be empty.
  # Set it to NO on Release-like configs.
  if name =~ /(release|archive|appstore|production)/i
    puts "  • #{name}: SKIP_INSTALL => NO"
    bs["SKIP_INSTALL"] = "NO"
    bs["INSTALL_PATH"] = "/Applications"
  end

  # Stabilise CI output a bit
  bs["ONLY_ACTIVE_ARCH"] = "NO"  if bs["ONLY_ACTIVE_ARCH"].to_s.empty?
  bs["DEAD_CODE_STRIPPING"] = "YES" if bs["DEAD_CODE_STRIPPING"].to_s.empty?
end

candidates.each do |t|
  puts "🛠  Patching target: #{t.name} (#{t.class})"

  # If it smells like an app (has Info.plist / bundle id) but product_type is wrong, force it.
  begin
    has_plist  = t.build_configurations.any? { |cfg| cfg.build_settings["INFOPLIST_FILE"].to_s.strip != "" }
    has_bundle = t.build_configurations.any? { |cfg| cfg.build_settings["PRODUCT_BUNDLE_IDENTIFIER"].to_s.strip != "" } ||
                 (!bundle_id.empty?)

    if t.respond_to?(:product_type) && (t.product_type.to_s.empty? || (!t.product_type.to_s.include?("application") && has_plist && has_bundle))
      puts "  • Setting product_type => com.apple.product-type.application (was: #{t.product_type})"
      t.product_type = "com.apple.product-type.application"
    end
  rescue => e
    warn "  ⚠️ Could not adjust product_type for #{t.name}: #{e}"
  end

  t.build_configurations.each do |cfg|
    patch_cfg!(cfg, bundle_id: bundle_id)
  end
end

# Also patch project-level build configurations as a safety net
project.build_configurations.each do |cfg|
  patch_cfg!(cfg, bundle_id: bundle_id)
end

project.save
puts "✅ Saved patched project: #{proj_path}"
