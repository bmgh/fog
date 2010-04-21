// [Fog-Gui Library - Public API]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Gui/Layout/BoxLayout.h>
#include <Fog/Gui/Widget.h>
#include <Fog/Gui/Layout/LayoutUtils.h>

FOG_IMPLEMENT_OBJECT(Fog::BoxLayout)

namespace Fog {

// ============================================================================
// [Fog::BoxLayout]
// ============================================================================

  BoxLayout::BoxLayout(Widget *parent, int margin, int spacing)
    : Layout(parent), _direction(LEFTTORIGHT)
  {
    setSpacing(spacing);
    setContentMargins(margin, margin, margin, margin);
  }

  BoxLayout::BoxLayout(int margin, int spacing) : _direction(LEFTTORIGHT)
  {
    setSpacing(spacing);
    setContentMargins(margin, margin, margin, margin);
  }

  BoxLayout::~BoxLayout()
  {
    LayoutItem *item;
    while ((item = takeAt(0)))
      delete item;
  }

  uint32_t BoxLayout::getLayoutExpandingDirections() const
  {
    return ORIENTATION_HORIZONTAL;
  }

  void BoxLayout::setLayoutGeometry(const IntRect &rect)
  {
    Layout::setLayoutGeometry(rect);
    doLayout(rect);
  }



  // ============================================================================
  // [Fog::HBoxLayout]
  // ============================================================================

  void HBoxLayout::calculateLayoutHint(LayoutHint& hint) {
    // Initialize
    int minWidth=0, width=0;
    int minHeight=0, height=0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();
      IntSize min = child->getLayoutMinimumSize();

      // Sum up widths
      width += hint.getWidth();

      // Detect if child is shrinkable or has percent width and update minWidth
      if (child->hasFlex()) {
        minWidth += min.getWidth();
      } else {
        minWidth += hint.getWidth();
      }

      // Build vertical margin sum
      int margin = child->getContentTopMargin() + child->getContentBottomMargin();

      // Find biggest height
      if ((hint.getHeight()+margin) > height) {
        height = hint.getHeight() + margin;
      }

      // Find biggest minHeight
      if ((min.getHeight()+margin) > minHeight) {
        minHeight = min.getHeight() + margin;
      }
    }

    // Respect gaps
    int gaps = (getContentLeftMargin() + calculateHorizontalGaps(_children, getSpacing(), true) + getContentRightMargin());

    hint._minimumSize.set(minWidth + gaps,minHeight+getContentTopMargin()+getContentBottomMargin());
    hint._sizeHint.set(width + gaps,height+getContentTopMargin()+getContentBottomMargin());
    hint._maximumSize.set(INT_MAX,INT_MAX);
  }

  int HBoxLayout::doLayout(const IntRect &rect)
  {
    if(!rect.isValid())
      return 0;
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    //support for Margin of Layout
    int gaps = getContentLeftMargin() + calculateHorizontalGaps(_children, getSpacing(), true) + getContentRightMargin();    
    int allocatedWidth = gaps;

    if(hasFlexItems()) {            
      //Prepare Values!
      for (sysuint_t i=0; i<getLength(); ++i)
      {
        LayoutItem* item = getAt(i);
        int hint = item->getLayoutSizeHint().getWidth();
        if(item->hasFlex()) {
          item->_layoutdata->_min = item->getLayoutMinimumSize().getWidth();
          item->_layoutdata->_max = item->getLayoutMaximumSize().getWidth();
          item->_layoutdata->_hint = hint;
          item->_layoutdata->_flex = (float)item->getFlex();
          item->_layoutdata->_offset = 0;
        }

        allocatedWidth += hint;
      }

      if(allocatedWidth != availWidth) {
        calculateFlexOffsets(_children, availWidth, allocatedWidth);
      }
    }

    int top, height, marginTop, marginBottom;
    int marginRight = -INT_MAX;    
    int spacing = getSpacing();

    // Render children and separators
    bool forward = isForward();

    register int i=-1;    
    int len = getLength()-1;
    int start = 0;

    if(!forward) {
      i = len+1;
      len = 0;
      start = len;
    }

    int left = collapseMargins(getContentLeftMargin(), getAt(start)->getContentLeftMargin());

    while(i != len)  {
      forward ? ++i : --i;
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();

      int width = hint.getWidth();
      width += child->_layoutdata->_offset;

      if(child->_layoutdata->_offset < 0) {
        width = width;
      }

      marginTop = getContentTopMargin() + child->getContentTopMargin();
      marginBottom = getContentBottomMargin() + child->getContentBottomMargin();

      // Find usable height
      height = Math::max<int>(child->getLayoutMinimumSize().getHeight(), Math::min(availHeight-marginTop-marginBottom, child->getLayoutMaximumSize().getHeight()));

      // Respect vertical alignment
      top = marginTop;

      // Add collapsed margin
      if (marginRight != -INT_MAX) {
        // Support margin collapsing
        left += collapseMargins(spacing, marginRight, child->getContentLeftMargin());
      }

      // Layout child
      child->setLayoutGeometry(IntRect(left, top, width, height));

      // Add width
      left += width;

      // Remember right margin (for collapsing)
      marginRight = child->getContentRightMargin();
    }

    return 0;
  }



  // ============================================================================
  // [Fog::VBoxLayout]
  // ============================================================================

  void VBoxLayout::calculateLayoutHint(LayoutHint& hint) {
    // Initialize
    int minWidth=0, width=0;
    int minHeight=0, height=0;

    // Iterate over children
    for (sysuint_t i=0; i<getLength(); ++i)
    {
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();
      IntSize min = child->getLayoutMinimumSize();

      // Sum up widths
      height += hint.getHeight();

      // Detect if child is shrinkable or has percent width and update minWidth
      if (child->hasFlex()) {
        minHeight += min.getHeight();
      } else {
        minHeight += hint.getHeight();
      }

      // Build vertical margin sum
      int margin = child->getContentTopMargin() + child->getContentBottomMargin();

      // Find biggest height
      if ((hint.getWidth()+margin) > width) {
        width = hint.getWidth() + width;
      }

      // Find biggest minHeight
      if ((min.getWidth()+margin) > minWidth) {
        minWidth = min.getWidth() + margin;
      }
    }

    // Respect gaps
    int gaps = (getContentLeftMargin() + calculateVerticalGaps(_children, getSpacing(), true) + getContentRightMargin());

    hint._minimumSize.set(minWidth+getContentLeftMargin()+getContentRightMargin(),minHeight+gaps);
    hint._sizeHint.set(width+getContentLeftMargin()+getContentRightMargin(),height+gaps);
    hint._maximumSize.set(INT_MAX,INT_MAX);
  }


  int VBoxLayout::doLayout(const IntRect &rect)
  {
    int availWidth = rect.getWidth();
    int availHeight = rect.getHeight();

    //support for Margin of Layout
    int gaps = getContentTopMargin() + calculateVerticalGaps(_children, getSpacing(), true) + getContentBottomMargin();
    int allocatedHeight = gaps;

    if(hasFlexItems()) {
      //Prepare Values!
      for (sysuint_t i=0; i<getLength(); ++i)
      {
        LayoutItem* item = getAt(i);
        int hint = item->getLayoutSizeHint().getHeight();
        if(item->hasFlex()) {
          item->_layoutdata->_min = item->getLayoutMinimumSize().getHeight();
          item->_layoutdata->_max = item->getLayoutMaximumSize().getHeight();
          item->_layoutdata->_hint = hint;
          item->_layoutdata->_flex = (float)item->getFlex();
          item->_layoutdata->_offset = 0;
        }

        allocatedHeight += hint;
      }

      if(allocatedHeight != availHeight) {
        calculateFlexOffsets(_children, availHeight, allocatedHeight);
      }
    }

    int left, height, marginLeft, marginRight;
    int marginBottom = -INT_MAX;    
    int spacing = getSpacing();

    // Render children and separators
    bool forward = isForward();

    register int i=-1;    
    int len = getLength()-1;
    int start = 0;

    if(!forward) {
      i = len+1;
      len = 0;
      start = len;
    }

    int top = collapseMargins(getContentTopMargin(), getAt(start)->getContentTopMargin());

    while(i != len)  {
      forward ? ++i : --i;
      LayoutItem* child = getAt(i);
      IntSize hint = child->getLayoutSizeHint();

      height = hint.getHeight();
      height += child->_layoutdata->_offset;

      marginLeft = getContentLeftMargin() + child->getContentLeftMargin();
      marginRight = getContentRightMargin() + child->getContentRightMargin();

      // Find usable height
      int width = Math::max<int>(child->getLayoutMinimumSize().getWidth(), Math::min(availWidth-marginLeft-marginRight, child->getLayoutMaximumSize().getWidth()));

      // Respect vertical alignment
      left = marginLeft;

      // Add collapsed margin
      if (marginRight != -INT_MAX) {
        // Support margin collapsing
        top += collapseMargins(spacing, marginBottom, child->getContentTopMargin());
      }

      // Layout child
      child->setLayoutGeometry(IntRect(left, top, width, height));

      // Add width
      top += height;

      // Remember right margin (for collapsing)
      marginRight = child->getContentRightMargin();
    }

    return 0;
  }

} // Fog namespaces

