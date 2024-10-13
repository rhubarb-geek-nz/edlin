// Copyright (c) 2024 Roger Brown.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;

namespace nlstool
{
    internal class EnumEnumerator : List<int>
    {
        internal EnumEnumerator(Type type)
        {
            Array values = type.GetEnumValues();

            foreach (int value in values)
            {
                Add(value);
            }

            Sort();
        }
    }
}
