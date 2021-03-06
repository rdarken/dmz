#ifndef DMZ_TYPES_UUID_DOT_H
#define DMZ_TYPES_UUID_DOT_H

#include <dmzKernelExport.h>
#include <dmzTypesBase.h>
#include <dmzTypesString.h>

namespace dmz {

/*!

\brief Universally Unique Identifier (UUID) container class.
\ingroup Types
\details UUIDs are 128 bit numbers used to uniquely identify software objects and
components.  UUIDs may be generated by dmz::create_uuid.
\htmlonly Lua bindings are <a href="dmzlua.html#dmz.uuid">available</a>.
\endhtmlonly
\sa dmz::create_uuid

*/
   class UUID {

      public:
         UUID ();
         UUID (const UUID &Value);
         UUID (const UInt8 Value[16]);
         UUID (const String &Value);
         ~UUID ();

         UUID &operator= (const UUID &Value);
         Boolean operator== (const UUID &Value) const;
         Boolean operator!= (const UUID &Value) const;
         Boolean operator! () const;
         DMZ_BOOLEAN_OPERATOR;

         void clear ();
         Boolean from_string (const String &Value);
         String to_string () const;

         void from_array (const UInt8 Value[16]);
         void to_array (UInt8 value[16]) const;

         Boolean set_element (const Int32 Index, const UInt8 Value);
         UInt8 get_element (const Int32 Index) const;

      protected:
         UInt64 _data[2]; //!< UUID stored as 128 bit number.
   };
};


//! Base constructor.
inline
dmz::UUID::UUID () { clear (); }


//! Copy constructor.
inline
dmz::UUID::UUID (const UUID &Value) { *this = Value; }


//! Array constructor.
inline
dmz::UUID::UUID (const UInt8 Value[16]) {

   if (Value) { from_array (Value); } else  { clear (); }
}


/*!

\brief String constructor.
\details Constructor uses UUID::from_string to convert string to UUID.
\sa UUID::from_string.

*/
inline
dmz::UUID::UUID (const String &Value) { if (!from_string (Value)) { clear (); } }


//! Destructor
inline
dmz::UUID::~UUID () {;}


//! Assignment operator.
inline dmz::UUID &
dmz::UUID::operator= (const UUID &Value) {

   _data[0] = Value._data[0];
   _data[1] = Value._data[1];

   return *this;
}


//! Relational "equal to" operator.
inline dmz::Boolean
dmz::UUID::operator== (const UUID &Value) const {

   return (_data[0] == Value._data[0]) && (_data[1] == Value._data[1]);
}


//! Relational "not equal to" operator.
inline dmz::Boolean
dmz::UUID::operator!= (const UUID &Value) const {

   return (_data[0] != Value._data[0]) || (_data[1] != Value._data[1]);
}

//! Logical not operator.
inline dmz::Boolean
dmz::UUID::operator! () const {

   return !_data[0] && !_data[1];
}


//! Clears class by setting all fields to zero.
inline void
dmz::UUID::clear () { _data[0] = 0; _data[1] = 0; }


/*!

\brief Converts formatted string into a UUID.
\details The string must contain 16 hex value characters (i.e. 0-9, a-f, A-F).
Dash "-" characters are ignored. \n
The following are all valid UUID strings:
- E5C8F93D-C68B-44BC-92C8-40422874A315
- B-9-3-9-0-E-C-9-F-8-0-4-4-E-E-6-B-C-A-4-B-8-7-2-A-2-6-9-8-0-5-B
- 1CC8C32E752847C2A69AA1F6C1056F4D

*/
inline dmz::Boolean
dmz::UUID::from_string (const String &Value) {

   Boolean result (False);

   const String Lower (Value.get_lower ());

   UInt8 newData[16];

   Boolean doShift (True);

   const Int32 Length (Lower.get_length ());

   Int32 place (0);
   Int32 count (0);

   Boolean done (False);

   while (!done) {

      const char Element (Lower.get_char (place));

      UInt8 offset (0);

      if ((Element >= '0') && (Element <= '9')) { offset = '0'; }
      else if ((Element >= 'a') && (Element <= 'f')) { offset = 'a' - 10; }

      if (offset) {

         UInt8 next (Element - offset);

         if (doShift) { next = next << 4; newData[count] = 0; doShift = False; }
         else { doShift = True; }

         newData[count] |= next;

         if (doShift) { count++; }
      }
      else if (Element == '-') {

         // ignore!
      }
      else { done = True; count = 0; }

      place++;

      if (place >= Length) { done = True; }
   }

   if (count == 16) {

      from_array (newData);
      result = True;
   }

   return result;
}


/*!

\brief converts UUID to a dmz::String.
\return Returns a dmz::String with the UUID formatted as:
XXXXXXXX-XXXX-XXXX-XXXXXXXXXXXX where X is any valid hexadecimal number.
This function returns a string containing lowercase hexadecimal letters
(i.e. a, b, c, d, e, and f)

*/
inline dmz::String
dmz::UUID::to_string () const {

   String result ("00000000-0000-0000-0000-000000000000");

   Int32 place (0);

   UInt8 *data ((UInt8 *)_data);

   for (Int32 ix = 0; ix < 16; ix++) {

      const UInt8 Value1 (data[ix] & 0x0f);
      const UInt8 Value2 ((data[ix] >> 4) & 0x0f);

      const char V1 ((Value1 < 0x0A) ? Value1 + '0' : Value1 + 'a' - 10);
      const char V2 ((Value2 < 0x0A) ? Value2 + '0' : Value2 + 'a' - 10);

      result.set_char (place, V2); place++;
      result.set_char (place, V1); place++;

      if ((place == 8) || (place == 13) || (place == 18) || (place == 23)) {

         result.set_char (place, '-'); place++;
      }
   }

   return result;
}


//! Converts an array of 16 dmz::UInt8 values to a UUID.
inline void
dmz::UUID::from_array (const UInt8 Value[16]) {

   UInt8 *data ((UInt8 *)_data);
   if (Value) { for (Int32 ix = 0; ix < 16; ix++) { data[ix] = Value[ix]; } }
}


//! Converts the UUID to an array of 16 dmz::UInt8 values.
inline void
dmz::UUID::to_array (UInt8 value[16]) const {

   UInt8 *data ((UInt8 *)_data);
   if (value) { for (Int32 ix = 0; ix < 16; ix++) { value[ix] = data[ix]; } }
}


/*!

\brief Stores \a Value at \a Index in the UUID
\param[in] Index of value in UUID.
\param[in] Value to be set at index in UUID.
\return Return dmz::True if element is successfully set.

*/
inline dmz::Boolean
dmz::UUID::set_element (const Int32 Index, const UInt8 Value) {

   Boolean result (False);

   UInt8 *data ((UInt8 *)_data);
   if ((Index >= 0) && (Index < 16)) { data[Index] = Value; result = True; }

   return result;
}


/*!

\brief Returns \a Value stored at \a Index in the UUID
\param[in] Index of value in UUID.
\return Return the value of the UUID specified by \a Index. If \a Index is
less than zero of greater than or equal to 16 the return value is NULL.

*/
inline dmz::UInt8
dmz::UUID::get_element (const Int32 Index) const {

   UInt8 *data ((UInt8 *)_data);
   return ((Index >= 0) && (Index < 16)) ? data[Index] : 0;
}

#endif // DMZ_TYPES_UUID_DOT_H
