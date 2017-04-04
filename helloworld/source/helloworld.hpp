#pragma once
#include <service/blik_zay.hpp>

class HWData : public ZayObject
{
public:
    HWData();
    ~HWData();

public:
    String mMessage;
};
