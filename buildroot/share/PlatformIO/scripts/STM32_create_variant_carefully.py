#
# STM32_create_variant_carefully.py
#
# Copy one of the variants from buildroot/platformio/variants into
# the appropriate framework variants folder, so that its contents
# will be picked up by PlatformIO just like any other variant.
# 
# !!! Just copy the file, Don't remove dir. !!!
#

import pioutil
if pioutil.is_pio_build():
  import marlin
  from pathlib import Path
  
  env = marlin.env
  platform = env.PioPlatform()
  board = env.BoardConfig()
  
  FRAMEWORK_DIR = Path(platform.get_package_dir("framework-arduinoststm32"))
  assert FRAMEWORK_DIR.is_dir()
  
  source_root = Path("buildroot/share/PlatformIO/variants")
  assert source_root.is_dir()
  
  variant = board.get("build.variant")
  variant_dir = FRAMEWORK_DIR / "variants" / variant
  
  source_dir = source_root / variant
  assert source_dir.is_dir()
  
  if not source_dir.is_dir():
    variant_dir.mkdir()
  
  marlin.copytree(source_dir, variant_dir)