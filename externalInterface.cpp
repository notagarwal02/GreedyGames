#include <cstddef>
#include <baseCommands.h>
#include "CurrencyBseNse.h"
extern "C"
{

    std::string getVersion()
    {    
        return "2.3.0" ;
    }

  void getDriver(void * params)
  {
    printf("Calling StampedeOMMDriver \n");
    CurrencyBseNse::TestDriveStrategy(params);
  }
  std::string getFrontEndDesign()
  {
  char params_txt[] = {
  0x5b, 0x53, 0x59, 0x4d, 0x42, 0x4f, 0x4c, 0x5d, 0x0a, 0x53, 0x59, 0x4d,
  0x42, 0x4f, 0x4c, 0x20, 0x4c, 0x45, 0x47, 0x31, 0x3d, 0x55, 0x49, 0x4e,
  0x54, 0x36, 0x34, 0x3a, 0x43, 0x3a, 0x31, 0x3a, 0x53, 0x79, 0x6d, 0x62,
  0x6f, 0x6c, 0x20, 0x4c, 0x65, 0x67, 0x20, 0x31, 0x0a, 0x4f, 0x72, 0x64,
  0x65, 0x72, 0x20, 0x4d, 0x6f, 0x64, 0x65, 0x20, 0x31, 0x3d, 0x55, 0x43,
  0x48, 0x41, 0x52, 0x3a, 0x30, 0x3a, 0x30, 0x0a, 0x41, 0x63, 0x63, 0x20,
  0x44, 0x65, 0x74, 0x61, 0x69, 0x6c, 0x20, 0x31, 0x3d, 0x41, 0x43, 0x43,
  0x4f, 0x55, 0x4e, 0x54, 0x0a, 0x53, 0x59, 0x4d, 0x42, 0x4f, 0x4c, 0x20,
  0x4c, 0x45, 0x47, 0x32, 0x3d, 0x55, 0x49, 0x4e, 0x54, 0x36, 0x34, 0x3a,
  0x43, 0x3a, 0x31, 0x3a, 0x53, 0x79, 0x6d, 0x62, 0x6f, 0x6c, 0x20, 0x4c,
  0x65, 0x67, 0x20, 0x32, 0x0a, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x20, 0x4d,
  0x6f, 0x64, 0x65, 0x20, 0x32, 0x3d, 0x55, 0x43, 0x48, 0x41, 0x52, 0x3a,
  0x31, 0x3a, 0x30, 0x0a, 0x41, 0x63, 0x63, 0x20, 0x44, 0x65, 0x74, 0x61,
  0x69, 0x6c, 0x20, 0x32, 0x3d, 0x41, 0x43, 0x43, 0x4f, 0x55, 0x4e, 0x54,
  0x0a, 0x0a, 0x5b, 0x53, 0x54, 0x52, 0x41, 0x54, 0x45, 0x47, 0x59, 0x5f,
  0x50, 0x41, 0x52, 0x41, 0x4d, 0x53, 0x5d, 0x0a, 0x52, 0x65, 0x73, 0x65,
  0x74, 0x5f, 0x41, 0x6c, 0x6c, 0x5f, 0x51, 0x74, 0x79, 0x3d, 0x42, 0x4f,
  0x4f, 0x4c, 0x3a, 0x30, 0x3a, 0x43, 0x68, 0x65, 0x63, 0x6b, 0x42, 0x6f,
  0x78, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x53, 0x45, 0x50, 0x41, 0x52, 0x41,
  0x54, 0x4f, 0x52, 0x3d, 0x31, 0x0a, 0x44, 0x65, 0x70, 0x74, 0x68, 0x3d,
  0x43, 0x4f, 0x4d, 0x42, 0x4f, 0x3a, 0x46, 0x69, 0x72, 0x73, 0x74, 0x3a,
  0x53, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x3a, 0x30, 0x3a, 0x43, 0x6f, 0x6d,
  0x62, 0x6f, 0x42, 0x6f, 0x78, 0x3a, 0x30, 0x3a, 0x30, 0x3a, 0x31, 0x0a,
  0x42, 0x75, 0x79, 0x44, 0x69, 0x66, 0x66, 0x3d, 0x46, 0x4c, 0x4f, 0x41,
  0x54, 0x3a, 0x44, 0x53, 0x50, 0x49, 0x4e, 0x42, 0x4f, 0x58, 0x3a, 0x2d,
  0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x2e, 0x30, 0x30, 0x30, 0x30,
  0x3a, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x2e, 0x30, 0x30, 0x30,
  0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30,
  0x30, 0x32, 0x35, 0x3a, 0x34, 0x3a, 0x42, 0x75, 0x79, 0x44, 0x69, 0x66,
  0x66, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x53, 0x65, 0x6c, 0x6c, 0x44, 0x69,
  0x66, 0x66, 0x3d, 0x46, 0x4c, 0x4f, 0x41, 0x54, 0x3a, 0x44, 0x53, 0x50,
  0x49, 0x4e, 0x42, 0x4f, 0x58, 0x3a, 0x2d, 0x39, 0x39, 0x39, 0x39, 0x39,
  0x39, 0x39, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x39, 0x39, 0x39, 0x39,
  0x39, 0x39, 0x39, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30,
  0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x32, 0x35, 0x3a, 0x34,
  0x3a, 0x53, 0x65, 0x6c, 0x6c, 0x44, 0x69, 0x66, 0x66, 0x3a, 0x31, 0x3a,
  0x30, 0x0a, 0x42, 0x75, 0x79, 0x54, 0x6f, 0x74, 0x61, 0x6c, 0x4c, 0x6f,
  0x74, 0x3d, 0x55, 0x49, 0x4e, 0x54, 0x36, 0x34, 0x3a, 0x53, 0x50, 0x49,
  0x4e, 0x42, 0x4f, 0x58, 0x3a, 0x30, 0x3a, 0x39, 0x39, 0x39, 0x39, 0x39,
  0x39, 0x39, 0x3a, 0x30, 0x3a, 0x31, 0x3a, 0x42, 0x75, 0x79, 0x54, 0x6f,
  0x74, 0x61, 0x6c, 0x4c, 0x6f, 0x74, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x53,
  0x65, 0x6c, 0x6c, 0x54, 0x6f, 0x74, 0x61, 0x6c, 0x4c, 0x6f, 0x74, 0x3d,
  0x55, 0x49, 0x4e, 0x54, 0x36, 0x34, 0x3a, 0x53, 0x50, 0x49, 0x4e, 0x42,
  0x4f, 0x58, 0x3a, 0x30, 0x3a, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
  0x3a, 0x30, 0x3a, 0x31, 0x3a, 0x53, 0x65, 0x6c, 0x6c, 0x54, 0x6f, 0x74,
  0x61, 0x6c, 0x4c, 0x6f, 0x74, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x4d, 0x61,
  0x78, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x4c, 0x6f, 0x74, 0x3d, 0x55, 0x49,
  0x4e, 0x54, 0x36, 0x34, 0x3a, 0x53, 0x50, 0x49, 0x4e, 0x42, 0x4f, 0x58,
  0x3a, 0x30, 0x3a, 0x33, 0x30, 0x30, 0x3a, 0x30, 0x3a, 0x31, 0x3a, 0x4d,
  0x61, 0x78, 0x4f, 0x72, 0x64, 0x65, 0x72, 0x4c, 0x6f, 0x74, 0x3a, 0x31,
  0x3a, 0x30, 0x0a, 0x53, 0x45, 0x50, 0x41, 0x52, 0x41, 0x54, 0x4f, 0x52,
  0x3d, 0x31, 0x0a, 0x54, 0x72, 0x69, 0x67, 0x67, 0x65, 0x72, 0x50, 0x72,
  0x69, 0x63, 0x65, 0x3d, 0x46, 0x4c, 0x4f, 0x41, 0x54, 0x3a, 0x44, 0x53,
  0x50, 0x49, 0x4e, 0x42, 0x4f, 0x58, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x30,
  0x30, 0x3a, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x2e, 0x30, 0x30,
  0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e,
  0x30, 0x30, 0x32, 0x35, 0x3a, 0x34, 0x3a, 0x54, 0x72, 0x69, 0x67, 0x67,
  0x65, 0x72, 0x50, 0x72, 0x69, 0x63, 0x65, 0x3a, 0x31, 0x3a, 0x30, 0x0a,
  0x53, 0x74, 0x6f, 0x70, 0x4c, 0x6f, 0x73, 0x73, 0x3d, 0x46, 0x4c, 0x4f,
  0x41, 0x54, 0x3a, 0x44, 0x53, 0x50, 0x49, 0x4e, 0x42, 0x4f, 0x58, 0x3a,
  0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x39, 0x39, 0x39, 0x39, 0x39,
  0x39, 0x39, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30,
  0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x32, 0x35, 0x3a, 0x34, 0x3a,
  0x53, 0x74, 0x6f, 0x70, 0x4c, 0x6f, 0x73, 0x73, 0x3a, 0x31, 0x3a, 0x30,
  0x0a, 0x54, 0x6f, 0x6c, 0x65, 0x72, 0x61, 0x6e, 0x63, 0x65, 0x3d, 0x46,
  0x4c, 0x4f, 0x41, 0x54, 0x3a, 0x44, 0x53, 0x50, 0x49, 0x4e, 0x42, 0x4f,
  0x58, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x39, 0x39, 0x39,
  0x39, 0x39, 0x39, 0x39, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e,
  0x30, 0x30, 0x30, 0x30, 0x3a, 0x30, 0x2e, 0x30, 0x30, 0x32, 0x35, 0x3a,
  0x34, 0x3a, 0x54, 0x6f, 0x6c, 0x65, 0x72, 0x61, 0x6e, 0x63, 0x65, 0x3a,
  0x31, 0x3a, 0x30, 0x0a, 0x49, 0x73, 0x42, 0x69, 0x64, 0x64, 0x69, 0x6e,
  0x67, 0x3d, 0x53, 0x54, 0x52, 0x49, 0x4e, 0x47, 0x3a, 0x59, 0x65, 0x73,
  0x3a, 0x6c, 0x69, 0x6e, 0x65, 0x45, 0x64, 0x69, 0x74, 0x3a, 0x31, 0x3a,
  0x30, 0x3a, 0x30, 0x0a, 0x54, 0x6f, 0x74, 0x61, 0x6c, 0x42, 0x75, 0x79,
  0x54, 0x72, 0x61, 0x64, 0x65, 0x64, 0x3d, 0x53, 0x54, 0x52, 0x49, 0x4e,
  0x47, 0x3a, 0x30, 0x3a, 0x6c, 0x69, 0x6e, 0x65, 0x45, 0x64, 0x69, 0x74,
  0x3a, 0x31, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x54, 0x6f, 0x74, 0x61, 0x6c,
  0x53, 0x65, 0x6c, 0x6c, 0x54, 0x72, 0x61, 0x64, 0x65, 0x64, 0x3d, 0x53,
  0x54, 0x52, 0x49, 0x4e, 0x47, 0x3a, 0x30, 0x3a, 0x6c, 0x69, 0x6e, 0x65,
  0x45, 0x64, 0x69, 0x74, 0x3a, 0x31, 0x3a, 0x31, 0x3a, 0x30, 0x0a, 0x0a,
  0x5b, 0x4f, 0x54, 0x48, 0x45, 0x52, 0x5d, 0x0a
};
unsigned int params_txt_len = 968;

    return std::string(params_txt,params_txt_len);
  }
}
