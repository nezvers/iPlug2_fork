#pragma once

/**
 * @file
 * @brief A collection of IControls for common UI widgets, such as knobs, sliders, switches
 */

#include "IControl.h"

/**
 * \defgroup Controls IGraphics::IControls
 * @{
 */

#pragma mark - Base Controls

/** Parent for knobs, to handle mouse action and ballistics. */
class IKnobControlBase : public IControl
{
public:
  IKnobControlBase(IPlugBaseGraphics& plug, IRECT rect, int paramIdx = kNoParameter,
                   EDirection direction = kVertical, double gearing = DEFAULT_GEARING)
  : IControl(plug, rect, paramIdx)
  , mDirection(direction)
  , mGearing(gearing)
  {}
  
  virtual ~IKnobControlBase() {}
  
  void SetGearing(double gearing) { mGearing = gearing; }
  virtual void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  virtual void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;
  
protected:
  EDirection mDirection;
  double mGearing;
};

/** Parent for buttons/switch controls */
class IButtonControlBase : public IControl
{
public:
  IButtonControlBase(IPlugBaseGraphics& plug, IRECT rect, int paramIdx = kNoParameter,  std::function<void(IGraphics*)> actionFunc = nullptr,
                     uint32_t numStates = 2)
  : IControl(plug, rect, paramIdx, actionFunc)
  {
    if(paramIdx > -1)
      mNumStates = (uint32_t) mPlug.GetParam(paramIdx)->GetRange() + 1;
    else
      mNumStates = numStates;
    
    assert(mNumStates > 1);
  }
  
  virtual ~IButtonControlBase() {}
  
  virtual void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    if(mNumStates == 2)
      mValue = !mValue;
    else
    {
      const float step = 1.f/float(mNumStates) - 1.;
      mValue += step;
      mValue = fmod(1., mValue);
    }
    
    if (mActionFunc != nullptr)
      mActionFunc(GetGUI());
    
    SetDirty();
  }
  
protected:
  uint32_t mNumStates;
};

#pragma mark - Vector Controls

/** A vector switch control. Click to cycle through states. */
class IVSwitchControl : public IButtonControlBase
{
public:
  IVSwitchControl(IPlugBaseGraphics& plug, IRECT rect, int paramIdx = kNoParameter, std::function<void(IGraphics*)> actionFunc = nullptr,
                  const IColor& fgColor = COLOR_BLACK, const IColor& bgColor = COLOR_WHITE,
                  uint32_t numStates = 2, EDirection dir = kVertical)
  : IButtonControlBase(plug, rect, paramIdx, actionFunc, numStates)
  , mFGColor(fgColor)
  , mBGColor(bgColor)
  , mDirection(dir)
  {
    mStep = 1.f/float(mNumStates) - 1.;
  }
  
  ~IVSwitchControl() {}
  
  void Draw(IGraphics& graphics)  override
  {
    const int state = mValue / mStep;
    
    graphics.FillRect(mBGColor, mRECT);
    
    IRECT handle;
    
    if(mDirection == kHorizontal)
      handle = mRECT.SubRectHorizontal(mNumStates, state);
    if(mDirection == kVertical)
      handle = mRECT.SubRectVertical(mNumStates, state);
    
    handle = handle.GetPadded(-10);
    
    graphics.FillRect(mFGColor, handle);
  }
  
private:
  float mStep;
  IColor mFGColor;
  IColor mBGColor;
  EDirection mDirection;
};

/** A vector knob control */
class IVKnobControl : public IKnobControlBase
{
public:
  IVKnobControl(IPlugBaseGraphics& plug, IRECT rect, int paramIdx, const IColor& fgcolor = DEFAULT_FGCOLOR, const IColor& bgcolor = DEFAULT_BGCOLOR, float rMin = 0.f, float rMax = 1.f, float aMin = -135.f, float aMax = 135.f, EDirection direction = kVertical, double gearing = DEFAULT_GEARING);
  ~IVKnobControl() {}
  
  void Draw(IGraphics& graphics) override;
  
protected:
  IColor mFGColor, mBGColor;
  float mAngleMin, mAngleMax, mInnerRadius, mOuterRadius;
};

#pragma mark - Bitmap Controls

/** Display monospace bitmap font text */
// TODO: fix Centre/Right aligned behaviour when string exceeds bounds or should wrap onto new line
class IBTextControl : public ITextControl
{
public:
  IBTextControl(IPlugBaseGraphics& plug, IRECT rect, IBitmap& textBitmap, const IText& text = DEFAULT_TEXT, const char* str = "", int charWidth = 6, int charHeight = 12, int charOffset = 0, bool multiLine = false, bool vCenter = true, EBlendType bl = kBlendNone)
  : ITextControl(plug, rect, text, str)
  , mCharWidth(charWidth)
  , mCharHeight(charHeight)
  , mCharOffset(charOffset)
  , mMultiLine(multiLine)
  , mVCentre(vCenter)
  , mTextBitmap(textBitmap)
  {
    mStr.Set(str);
  }

  void Draw(IGraphics& graphics) override
  {
    graphics.DrawBitmapedText(mTextBitmap, mRECT, mText, &mBlend, mStr.Get(), mVCentre, mMultiLine, mCharWidth, mCharHeight, mCharOffset);
  }
  
protected:
  WDL_String mStr;
  int mCharWidth, mCharHeight, mCharOffset;
  bool mMultiLine;
  bool mVCentre;
  IBitmap mTextBitmap;
};

/**@}*/

