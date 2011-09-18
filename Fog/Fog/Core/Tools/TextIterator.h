// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_TEXTITERATOR_H
#define _FOG_CORE_TOOLS_TEXTITERATOR_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/CharData.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/TextChunk.h>

namespace Fog {

//! @addtogroup Fog_G2d_Text
//! @{

// ============================================================================
// [Fog::TextIterator]
// ============================================================================

//! @brief Text iterator.
struct FOG_NO_EXPORT TextIterator
{
  // --------------------------------------------------------------------------
  // [Text Iterator]
  // --------------------------------------------------------------------------

  FOG_INLINE TextIterator() :
    _pos(NULL),
    _end(NULL)
  {
  }

  explicit FOG_INLINE TextIterator(const StubW& s) { assign(s); }
  explicit FOG_INLINE TextIterator(const StringW& s) { assign(s); }
  explicit FOG_INLINE TextIterator(const TextChunk& s) { assign(s); }

  FOG_INLINE ~TextIterator()
  {
  }

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const CharW* getPos() const { return _pos; }
  FOG_INLINE const CharW* getEnd() const { return _end; }

  // --------------------------------------------------------------------------
  // [Assign]
  // --------------------------------------------------------------------------

  FOG_INLINE err_t assign(const StubW& s)
  {
    _pos = s.getData();
    _end = _pos + s.getComputedLength();

    return _detectInvalidString();
  }

  FOG_INLINE err_t assign(const StringW& s)
  {
    _pos = s.getData();
    _end = _pos + s.getLength();

    return _detectInvalidString();
  }

  FOG_INLINE err_t assign(const TextChunk& s)
  {
    _pos = s.getText().getData();
    _end = _pos;

    _pos += s.getRange().getStart();
    _end += s.getRange().getEnd();

    return _detectInvalidString();
  }

  //! @internal
  //!
  //! @brief Detect an invalid string sequence, called after @c assign().
  FOG_NO_INLINE err_t _detectInvalidString()
  {
    err_t err = ERR_OK;

    if (_pos != _end)
    {
      if (_pos[0].isHiSurrogate())
      {
        err = ERR_STRING_INVALID_INPUT;
        goto _Fail;
      }

      if (_end[-1].isHiSurrogate())
      {
        err = ERR_STRING_TRUNCATED;
        goto _Fail;
      }
    }

    return err;

_Fail:
    reset();
    return err;
  }

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  //! @brief Reset.
  FOG_INLINE void reset()
  {
    _pos = NULL;
    _end = NULL;
  }

  // --------------------------------------------------------------------------
  // [Interface]
  // --------------------------------------------------------------------------

  FOG_INLINE bool isInitialized() const
  {
    return _pos != NULL;
  }

  //! @brief Get whether the iterator has next character.
  FOG_INLINE bool hasNext() const
  {
    return _pos != _end;
  }

  //! @brief Get the next 16-bit unicode character.
  FOG_INLINE uint16_t nextU16()
  {
    FOG_ASSERT(_pos != _end);
    return *_pos++;
  }

  //! @brief Get the next 32-bit unicode character.
  //!
  //! This method does compositing of surrogate pairs, but not validates them
  //! (it's fast implementation). The string should be validated before the
  //! @c nextU32() method is called.
  FOG_INLINE uint32_t nextU32()
  {
    uint32_t uc0 = nextU16();

    if (FOG_UNLIKELY(CharW::isSurrogate(uc0)))
    {
      // Invalid position already checked by _detectInvalidString().
      // Other case is the runtime failure.
      uint32_t uc1 = nextU16();

      // Combine uc0 and uc1 to UCS4 code-point.
      // -0x35FDC00 == (0xD800 << 10) + 0xDC00 - 0x10000.
      uc0 = (uc0 << 10) + uc1 - 0x035FDC00;
    }

    return uc0;
  }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  const CharW* _pos;
  const CharW* _end;
};

//! @}

} // Fog namespace

// ============================================================================
// [Fog::TypeInfo<>]
// ============================================================================

_FOG_TYPE_DECLARE(Fog::TextIterator, Fog::TYPE_CATEGORY_SIMPLE)

// [Guard]
#endif // _FOG_CORE_TOOLS_TEXTITERATOR_H
