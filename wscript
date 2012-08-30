srcdir = "."
blddir = "build"
single = True
modules = ["tc_all", "tc_hdb", "tc_bdb", "tc_fdb", "tc_tdb", "tc_adb"]
VERSION = "0.1.1"

import Options
from shutil import copy
from os import unlink
from os.path import exists

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def genmod(bld, target, source):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = target
  sources = []
  for s in source:
    sources.append("src/" + s + ".cc")
  obj.sources = " ".join(sources)
  obj.includes = ["."]
  obj.defines = "__STDC_LIMIT_MACROS"
  obj.lib = ["tokyocabinet"]

def build(bld):
  if single:
    genmod(bld, "tc_all", modules)
  else:
    for m in modules:
      genmod(bld, m, [m])

def shutdown():
  if Options.commands["clean"]:
    for m in modules:
      f = m + ".node"
      if exists(f): unlink(f)
  if Options.commands["build"]:
    for m in modules:
      for p in ["default", "Release"]:
        f = blddir + "/" + p + "/" + m + ".node"
        if exists(f): copy(f, '.')
