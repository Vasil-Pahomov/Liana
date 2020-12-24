#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
//https://github.com/Makuna/NeoPixelBus/issues/103

enum NeoPixelType
{
  NeoPixelType_Rgb  = 0,
  NeoPixelType_Grb  = 1,
  NeoPixelType_Rbg  = 2,
  NeoPixelType_Rgbw = 3,
  NeoPixelType_Grbw = 4
};

class NeoPixelWrapper
{
public:
  NeoPixelWrapper() :
      // initialize each member to null
      _pRgb(NULL),
      _pGrb(NULL),
      _pRbg(NULL),
      _pRgbw(NULL),
      _pGrbw(NULL),
      _type(NeoPixelType_Rgb)
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
        _pRgb = new NeoPixelBus<NeoRgbFeature,NeoEsp8266Uart1800KbpsMethod>(countPixels, pin);
        _pRgb->Begin();
        Serial.println("RGB");
      break;

      case NeoPixelType_Grb:
        _pGrb = new NeoPixelBus<NeoGrbFeature,NeoEsp8266Uart1800KbpsMethod>(countPixels, pin);
        _pGrb->Begin();
        Serial.println("GRB");
      break;

      case NeoPixelType_Rbg:
        _pRbg = new NeoPixelBus<NeoRbgFeature,NeoEsp8266Uart1800KbpsMethod>(countPixels, pin);
        _pRbg->Begin();
        Serial.println("RBG");
      break;

      case NeoPixelType_Rgbw:
        _pRgbw = new NeoPixelBus<NeoRgbwFeature,NeoEsp8266Uart1800KbpsMethod>(countPixels, pin);
        _pRgbw->Begin();
        Serial.println("RGBW");
      break;

      case NeoPixelType_Grbw:
        _pGrbw = new NeoPixelBus<NeoGrbwFeature,NeoEsp8266Uart1800KbpsMethod>(countPixels, pin);
        _pGrbw->Begin();
        Serial.println("GRBW");
      break;
    }
  }

  void Show()
  {
    switch (_type) {
      case NeoPixelType_Rgb:  _pRgb->Show();   break;
      case NeoPixelType_Grb:  _pGrb->Show();   break;
      case NeoPixelType_Rbg:  _pRbg->Show();   break;
      case NeoPixelType_Rgbw: _pRgbw->Show();  break;
      case NeoPixelType_Grbw: _pGrbw->Show();  break;
    }
  }
  bool CanShow() const
  {
    switch (_type) {
      case NeoPixelType_Rgb:  _pRgb->CanShow();  break;
      case NeoPixelType_Grb:  _pGrb->CanShow();  break;
      case NeoPixelType_Rbg:  _pRbg->CanShow();  break;
      case NeoPixelType_Rgbw: _pRgbw->CanShow(); break;
      case NeoPixelType_Grbw: _pGrbw->CanShow(); break;
    }
  }

  // replicate all the calls like the above
    bool IsDirty() const
    {
    }

    void Dirty()
    {
    }

    void ResetDirty()
    {
    }

    uint8_t* Pixels() const
    {
    }

    size_t PixelsSize() const
    {
    }

    size_t PixelSize() const
    {
    }

    uint16_t PixelCount() const
    {
    }

    void SetPixelColor(uint16_t indexPixel, RgbColor color)
    {
      switch (_type) {
        case NeoPixelType_Rgb: _pRgb->SetPixelColor(indexPixel, color);   break;
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
  NeoPixelBus<NeoRgbFeature,NeoEsp8266Uart1800KbpsMethod>*  _pRgb;
  NeoPixelBus<NeoGrbFeature,NeoEsp8266Uart1800KbpsMethod>*  _pGrb;
  NeoPixelBus<NeoRbgFeature,NeoEsp8266Uart1800KbpsMethod>*  _pRbg;
  NeoPixelBus<NeoRgbwFeature,NeoEsp8266Uart1800KbpsMethod>* _pRgbw;
  NeoPixelBus<NeoGrbwFeature,NeoEsp8266Uart1800KbpsMethod>* _pGrbw;

  void cleanup()
  {
    switch (_type) {
      case NeoPixelType_Rgb:  delete _pRgb ; _pRgb  = NULL; break;
      case NeoPixelType_Grb:  delete _pGrb ; _pGrb  = NULL; break;
      case NeoPixelType_Rbg:  delete _pRbg ; _pRbg  = NULL; break;
      case NeoPixelType_Rgbw: delete _pRgbw; _pRgbw = NULL; break;
      case NeoPixelType_Grbw: delete _pGrbw; _pGrbw = NULL; break;
    }
  }
};
