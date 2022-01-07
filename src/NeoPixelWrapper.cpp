#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
//https://github.com/Makuna/NeoPixelBus/issues/103


#if defined(ESP8266)
#define NP_METHOD NeoEsp8266Uart1800KbpsMethod
#elif defined(ESP32)
#define NP_METHOD NeoEsp32Rmt0800KbpsMethod
#else
#error "Unsupported board class"
#endif

enum NeoPixelType
{
  NeoPixelType_Rgb  = 0,
  NeoPixelType_Grb  = 1,
  NeoPixelType_Rbg  = 2,
  NeoPixelType_Rgbw = 3,
  NeoPixelType_Grbw = 4,
  NeoPixelType_Brg = 5,
  NeoPixelType_Bgr = 6
};

class NeoPixelWrapper
{
public:
  NeoPixelWrapper() :
      // initialize each member to null
      _type(NeoPixelType_Rgb),
      _pRgb(NULL),
      _pBrg(NULL),
      _pGrb(NULL),
      _pRbg(NULL),
      _pRgbw(NULL),
      _pGrbw(NULL)
  {

  }

  ~NeoPixelWrapper()
  {
    cleanup();
  }

  void Begin(uint16_t type, uint16_t countPixels, uint8_t pin)
  {
    cleanup();
    _type = (NeoPixelType)type;

    switch (_type) {

      case NeoPixelType_Rgb:
        _pRgb = new NeoPixelBus<NeoRgbFeature,NP_METHOD>(countPixels, pin);
        _pRgb->Begin();
        Serial.println("RGB");
      break;

      case NeoPixelType_Brg:
        _pBrg = new NeoPixelBus<NeoBrgFeature,NP_METHOD>(countPixels, pin);
        _pBrg->Begin();
        Serial.println("BRG");
      break;

      case NeoPixelType_Bgr:
        _pBgr = new NeoPixelBus<NeoBgrFeature,NP_METHOD>(countPixels, pin);
        _pBgr->Begin();
        Serial.println("BGR");
      break;

      case NeoPixelType_Grb:
        _pGrb = new NeoPixelBus<NeoGrbFeature,NP_METHOD>(countPixels, pin);
        _pGrb->Begin();
        Serial.println("GRB");
      break;

      case NeoPixelType_Rbg:
        _pRbg = new NeoPixelBus<NeoRbgFeature,NP_METHOD>(countPixels, pin);
        _pRbg->Begin();
        Serial.println("RBG");
      break;

      case NeoPixelType_Rgbw:
        _pRgbw = new NeoPixelBus<NeoRgbwFeature,NP_METHOD>(countPixels, pin);
        _pRgbw->Begin();
        Serial.println("RGBW");
      break;

      case NeoPixelType_Grbw:
        _pGrbw = new NeoPixelBus<NeoGrbwFeature,NP_METHOD>(countPixels, pin);
        _pGrbw->Begin();
        Serial.println("GRBW");
      break;
    }
  }

  void Show()
  {
    switch (_type) {
      case NeoPixelType_Rgb:  _pRgb->Show();   break;
      case NeoPixelType_Brg:  _pBrg->Show();   break;
      case NeoPixelType_Bgr:  _pBgr->Show();   break;
      case NeoPixelType_Grb:  _pGrb->Show();   break;
      case NeoPixelType_Rbg:  _pRbg->Show();   break;
      case NeoPixelType_Rgbw: _pRgbw->Show();  break;
      case NeoPixelType_Grbw: _pGrbw->Show();  break;
    }
  }

  bool CanShow() const
  {
    switch (_type) {
      case NeoPixelType_Rgb:  return _pRgb->CanShow();
      case NeoPixelType_Brg:  return _pBrg->CanShow();
      case NeoPixelType_Bgr:  return _pBgr->CanShow();
      case NeoPixelType_Grb:  return _pGrb->CanShow();
      case NeoPixelType_Rbg:  return _pRbg->CanShow();
      case NeoPixelType_Rgbw: return _pRgbw->CanShow();
      case NeoPixelType_Grbw: return _pGrbw->CanShow();
      default: return false;
    }
  }

    void SetPixelColor(uint16_t indexPixel, RgbColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Brg: _pBrg->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Bgr: _pBgr->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Grb: _pGrb->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rbg: _pRbg->SetPixelColor(indexPixel, color);   break;
        case NeoPixelType_Rgbw:_pRgbw->SetPixelColor(indexPixel, color);  break;
        case NeoPixelType_Grbw:_pGrbw->SetPixelColor(indexPixel, color);  break;
      }
    }

    void SetPixelColor(uint16_t indexPixel, HslColor color)
    {
      //implement if needed
    }

    void SetPixelColor(uint16_t indexPixel, RgbwColor color)
    {
      //implement if needed
    }

    RgbColor GetPixelColor(uint16_t indexPixel) const
    {
      return 0;//implement if needed
    }

    void ClearTo(RgbColor color)
    {
    }

    void ClearTo(RgbwColor color)
    {
    }

    void ClearTo(RgbColor color, uint16_t first, uint16_t last)
    {
    }

    void ClearTo(RgbwColor color, uint16_t first, uint16_t last)
    {
    }

    void RotateLeft(uint16_t rotationCount)
    {
    }

    void RotateLeft(uint16_t rotationCount, uint16_t first, uint16_t last)
    {
    }

    void ShiftLeft(uint16_t shiftCount)
    {
    }

    void ShiftLeft(uint16_t shiftCount, uint16_t first, uint16_t last)
    {
    }

    void RotateRight(uint16_t rotationCount)
    {
    }

    void RotateRight(uint16_t rotationCount, uint16_t first, uint16_t last)
    {
    }

    void ShiftRight(uint16_t shiftCount)
    {
    }

    void ShiftRight(uint16_t shiftCount, uint16_t first, uint16_t last)
    {
    }

private:
  NeoPixelType _type;

  // have a member for every possible type
  NeoPixelBus<NeoRgbFeature,NP_METHOD>*  _pRgb;
  NeoPixelBus<NeoBrgFeature,NP_METHOD>*  _pBrg;
  NeoPixelBus<NeoBgrFeature,NP_METHOD>*  _pBgr;
  NeoPixelBus<NeoGrbFeature,NP_METHOD>*  _pGrb;
  NeoPixelBus<NeoRbgFeature,NP_METHOD>*  _pRbg;
  NeoPixelBus<NeoRgbwFeature,NP_METHOD>* _pRgbw;
  NeoPixelBus<NeoGrbwFeature,NP_METHOD>* _pGrbw;

  void cleanup()
  {
    switch (_type) {
      case NeoPixelType_Rgb:  delete _pRgb ; _pRgb  = NULL; break;
      case NeoPixelType_Brg:  delete _pBrg ; _pBrg  = NULL; break;
      case NeoPixelType_Bgr:  delete _pBrg ; _pBrg  = NULL; break;
      case NeoPixelType_Grb:  delete _pGrb ; _pGrb  = NULL; break;
      case NeoPixelType_Rbg:  delete _pRbg ; _pRbg  = NULL; break;
      case NeoPixelType_Rgbw: delete _pRgbw; _pRgbw = NULL; break;
      case NeoPixelType_Grbw: delete _pGrbw; _pGrbw = NULL; break;
    }
  }
};
