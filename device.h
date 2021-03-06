#ifndef IB_HEAD
#define IB_HEAD

#include <infiniband/verbs.h>
#include <rdma/rdma_cma.h>

#include <string>
#include <iostream>

class Device
{
  ibv_device *device;
  ibv_context *device_ctx;
  const char* name;

 public:
  ibv_device_attr *device_attr;
  explicit Device(ibv_device* d): device(d), device_attr(new ibv_device_attr)
  {
    name = ibv_get_device_name(device);
    device_ctx = ibv_open_device(device);
    int r = ibv_query_device(device_ctx, device_attr);
    if (r != 0) {
        std::cout << "failed to query device name: " << name << " ret: " << r << std::endl;
    } else {
        std::cout << "succeed to query device name: " << name << std::endl;
    }
  }

  ~Device() {
    ibv_close_device(device_ctx);
  }

  const char* get_name()
  {
    return name;
  }

};


class DeviceList
{
  struct ibv_device ** device_list;
  struct ibv_context * device_context;
  int num;
  Device** devices;
  public:
  explicit DeviceList(void): device_list(ibv_get_device_list(&num))
  {
    devices = new Device*[num];

    for (int i = 0;i < num; ++i) {
      devices[i] = new Device(device_list[i]);
    }
  }

  ~DeviceList()
  {
    for (int i = 0; i < num; ++i) {
      delete devices[i];
    }

    delete []devices;
    ibv_free_device_list(device_list);
  }

  Device* get_device(const char* device_name)
  {
    for (int i = 0; i < num; ++i) {
      if (!strlen(device_name) || !strcmp(device_name, devices[i]->get_name())) {
        return devices[i];
      }
    }
    return NULL;
  }
};

#endif
