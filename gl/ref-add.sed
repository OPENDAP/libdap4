/^# Packages using this file: / {
  s/# Packages using this file://
  ta
  :a
  s/ libdap / libdap /
  tb
  s/ $/ libdap /
  :b
  s/^/# Packages using this file:/
}
