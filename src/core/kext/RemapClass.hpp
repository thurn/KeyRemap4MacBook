#ifndef REMAPCLASS_HPP
#define REMAPCLASS_HPP

#include "bridge.h"
#include "remap.hpp"
#include "RemapFunc/KeyToKey.hpp"
#include "RemapFunc/KeyToConsumer.hpp"
#include "RemapFunc/KeyToPointingButton.hpp"
#include "RemapFunc/ConsumerToConsumer.hpp"
#include "RemapFunc/ConsumerToKey.hpp"
#include "RemapFunc/DoublePressModifier.hpp"
#include "RemapFunc/DropKeyAfterRemap.hpp"
#include "RemapFunc/DropPointingRelativeCursorMove.hpp"
#include "RemapFunc/ForceNumLockOn.hpp"
#include "RemapFunc/HoldingKeyToKey.hpp"
#include "RemapFunc/IgnoreMultipleSameKeyPress.hpp"
#include "RemapFunc/KeyOverlaidModifier.hpp"
#include "RemapFunc/PointingButtonToKey.hpp"
#include "RemapFunc/PointingButtonToPointingButton.hpp"
#include "RemapFunc/PointingRelativeToScroll.hpp"
#include "RemapFunc/SimultaneousKeyPresses.hpp"
#include "RemapFunc/SetKeyboardType.hpp"
#include "RemapFilter/FilterUnion.hpp"

namespace org_pqrs_KeyRemap4MacBook {
  class RemapClass {
  public:
    enum {
      MAX_CONFIG_COUNT = 10000,
      MAX_ALLOCATION_COUNT = (8 * 1024 * 1024) / sizeof(uint32_t), // 8MB
    };

    class Item {
    public:
      Item(const uint32_t* vec, size_t length);
      ~Item(void);
      void append_filter(const uint32_t* vec, size_t length);

      // --------------------
      bool remap(RemapParams& remapParams);
      bool remap(RemapConsumerParams& remapParams);
      bool remap(RemapPointingParams_relative& remapParams);
      // for DropKeyAfterRemap
      bool drop(const Params_KeyboardEventCallBack& params);
      //
      bool remap_SimultaneousKeyPresses(void);
      //
      bool remap_setkeyboardtype(KeyboardType& keyboardType);
      //
      bool remap_forcenumlockon(ListHookedKeyboard::Item* item);

    private:
      bool isblocked(void);

      uint32_t type_;

      union {
        RemapFunc::KeyToKey* keyToKey;
        RemapFunc::KeyToConsumer* keyToConsumer;
        RemapFunc::KeyToPointingButton* keyToPointingButton;
        RemapFunc::ConsumerToConsumer* consumerToConsumer;
        RemapFunc::ConsumerToKey* consumerToKey;
        RemapFunc::DoublePressModifier* doublePressModifier;
        RemapFunc::DropKeyAfterRemap* dropKeyAfterRemap;
        RemapFunc::DropPointingRelativeCursorMove* dropPointingRelativeCursorMove;
        RemapFunc::ForceNumLockOn* forceNumLockOn;
        RemapFunc::HoldingKeyToKey* holdingKeyToKey;
        RemapFunc::IgnoreMultipleSameKeyPress* ignoreMultipleSameKeyPress;
        RemapFunc::KeyOverlaidModifier* keyOverlaidModifier;
        RemapFunc::PointingButtonToKey* pointingButtonToKey;
        RemapFunc::PointingButtonToPointingButton* pointingButtonToPointingButton;
        RemapFunc::PointingRelativeToScroll* pointingRelativeToScroll;
        RemapFunc::SimultaneousKeyPresses* simultaneousKeyPresses;
        RemapFunc::SetKeyboardType* setKeyboardType;
      } p_;

      RemapFilter::Vector_FilterUnionPointer* filters_;
    };
    typedef Item* ItemPointer;
    DECLARE_VECTOR(ItemPointer);

    // ----------------------------------------------------------------------
    RemapClass(const uint32_t* const initialize_vector, uint32_t vector_size);
    ~RemapClass(void);

    void remap_setkeyboardtype(KeyboardType& keyboardType);
    void remap_forcenumlockon(ListHookedKeyboard::Item* item);
    void remap_key(RemapParams& remapParams);
    void remap_consumer(RemapConsumerParams& remapParams);
    void remap_pointing(RemapPointingParams_relative& remapParams);
    bool remap_simultaneouskeypresses(void);
    bool remap_dropkeyafterremap(const Params_KeyboardEventCallBack& params);
    const char* get_statusmessage(void);
    bool enabled(void) const { return enabled_; }
    void setEnabled(bool newval) { enabled_ = newval; }
    void toggleEnabled(void) { enabled_ = ! enabled_; }
    bool is_simultaneouskeypresses(void);

    static void log_allocation_count(void);
    static void reset_allocation_count(void);

  private:
    Vector_ItemPointer items_;
    char* statusmessage_;
    bool enabled_;
    bool is_simultaneouskeypresses_;

    static int allocation_count_;
  };
  typedef RemapClass* RemapClassPointer;
  DECLARE_VECTOR(RemapClassPointer);

  // ================================================================================
  namespace RemapClassManager {
    void initialize(IOWorkLoop& workloop);
    void terminate(void);

    bool load_remapclasses_initialize_vector(const uint32_t* const remapclasses_initialize_vector, mach_vm_size_t vector_size);
    bool set_config(const int32_t* const config_vector, mach_vm_size_t config_size);

    // call after setting enable/disable status is changed.
    // (PreferencesPane, VK_CONFIG)
    void refresh(void);

    void remap_setkeyboardtype(KeyboardType& keyboardType);
    void remap_forcenumlockon(ListHookedKeyboard::Item* item);
    void remap_key(RemapParams& remapParams);
    void remap_consumer(RemapConsumerParams& remapParams);
    void remap_pointing(RemapPointingParams_relative& remapParams);

    // return true if EventInputQueue::queue_ is changed.
    bool remap_simultaneouskeypresses(void);

    // return true if dropped.
    bool remap_dropkeyafterremap(const Params_KeyboardEventCallBack& params);

    bool isEventInputQueueDelayEnabled(void);

    bool isEnabled(size_t configindex);
  };
}

#endif
