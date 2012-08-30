srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

def genmod(bld, target, source):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = target
  if source == None:
    source = [target]
  for i in range(len(source)):
    source[i] = "src/" + source[i] + ".cc"
  obj.sources = " ".join(source)
  obj.includes = ["."]
  obj.defines = "__STDC_LIMIT_MACROS"
  obj.lib = ["tokyocabinet"]

def build(bld):
  if True:
    genmod(bld, "tc_all", ["tc_all", "tc_hdb", "tc_bdb", "tc_fdb", "tc_tdb", "tc_adb"])
  else:
    genmod(bld, "tc_hdb")
    genmod(bld, "tc_bdb")
    genmod(bld, "tc_fdb")
    genmod(bld, "tc_tdb")
    genmod(bld, "tc_adb")
