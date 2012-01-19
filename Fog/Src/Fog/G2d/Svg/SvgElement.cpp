// [Fog-G2d]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/G2d/Svg/SvgDocument.h>
#include <Fog/G2d/Svg/SvgElement.h>
#include <Fog/G2d/Svg/SvgVisitor.h>

namespace Fog {

// ============================================================================
// [Fog::SvgElement]
// ============================================================================

SvgElement::SvgElement(const ManagedStringW& tagName, uint32_t svgType) :
  XmlElement(tagName),
  _boundingBox(0.0f, 0.0f, 0.0f, 0.0f),
  _boundingBoxDirty(true),
  _visible(true),
  _unused_0(0),
  _unused_1(0)
{
  _nodeFlags &= ~(DOM_FLAG_MUTABLE_NAME);
  _ext.setExtension(DOM_EXT_GROUP_SVG, svgType);
}

SvgElement::~SvgElement()
{
  // Class that inherits us have to destroy all attributes.
  FOG_ASSERT(_attributes.isEmpty());
}

XmlElement* SvgElement::clone() const
{
  SvgElement* e = reinterpret_cast<SvgElement*>(SvgDocument::createElementStatic(_tagName));
  if (e) _copyAttributes(e, const_cast<SvgElement*>(this));
  return e;
}

XmlAttribute* SvgElement::_createAttribute(const ManagedStringW& name) const
{
  return base::_createAttribute(name);
}

err_t SvgElement::onPrepare(SvgVisitor* visitor, SvgGState* state) const
{
  // Should be reimplemented.
  return ERR_OK;
}

err_t SvgElement::onProcess(SvgVisitor* visitor) const
{
  // Should be reimplemented.
  return ERR_OK;
}

err_t SvgElement::onPattern(SvgVisitor* visitor, SvgElement* obj, uint32_t paintType) const
{
  // Should be reimplemented if SvgElement is SvgPattern, SvgLinearGradient or
  // SvgRadialGradient.
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::onGeometryBoundingBox(BoxF& box, const TransformF* tr) const
{
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::onStrokeBoundingBox(BoxF& box, const PathStrokerParamsF& stroke, const TransformF* tr) const
{
  // TODO:
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::_visitContainer(SvgVisitor* visitor) const
{
  err_t err = ERR_OK;
  XmlElement* e;

  for (e = getFirstChild(); e; e = e->getNextSibling())
  {
    if (e->isExtensionGroupAndNode(DOM_EXT_GROUP_SVG, DOM_NODE_ELEMENT) && static_cast<SvgElement*>(e)->getVisible())
    {
      err = visitor->onVisit(static_cast<SvgElement*>(e));
      if (FOG_IS_ERROR(err)) break;
    }
  }

  return err;
}

StringW SvgElement::getStyle(const StringW& name) const
{
  return StringW();
}

err_t SvgElement::setStyle(const StringW& name, const StringW& value)
{
  return ERR_RT_INVALID_STATE;
}

err_t SvgElement::getBoundingBox(BoxF& box) const
{
  if (_boundingBoxDirty)
  {
    // TODO:
    FOG_RETURN_ON_ERROR(onGeometryBoundingBox(_boundingBox, NULL));
    _boundingBoxDirty = false;
  }

  box = _boundingBox;
  return ERR_OK;
}

err_t SvgElement::getBoundingBox(BoxF& box, const TransformF* tr) const
{
  if (tr == NULL) return getBoundingBox(box);

  switch (tr->getType())
  {
    case TRANSFORM_TYPE_IDENTITY:
      return getBoundingBox(box);

    case TRANSFORM_TYPE_TRANSLATION:
    case TRANSFORM_TYPE_SCALING:
    case TRANSFORM_TYPE_SWAP:
      FOG_RETURN_ON_ERROR(getBoundingBox(box));
      tr->mapBox(box, box);
      return ERR_OK;

    default:
      return onGeometryBoundingBox(box, tr);
  }
}

} // Fog namespace
