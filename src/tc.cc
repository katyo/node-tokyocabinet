#include "tc.hh"

namespace node {
  SUBMD_DECL(hdb) SUBMD_DECL(bdb) SUBMD_DECL(fdb) SUBMD_DECL(tdb) SUBMD_DECL(adb);
  ADDON_INIT(tc, SUBMD_INIT(hdb) SUBMD_INIT(bdb) SUBMD_INIT(fdb) SUBMD_INIT(tdb) SUBMD_INIT(adb));
}
