# meta-pointers

Did you know an `int` *(typically)* has an **alignment of 4?!** That means for any `int*` there's **2 perfectly good bits** that'll always be zero *(assuming you have a compiler made after 753 BC)*. What a **waste**. There are **children starving in Africa** who would kill to have those 2 bits.

`meta-pointers` is a single-file header-only utility for C++ that defines a template type `meta_ptr<T>` that acts as a `T*` but has additional meta data stored in those alignment bits. `meta_ptr<T>` will automatically detect the alignment of whatever type you give it and give you access to the correct number of meta bits (e.g. `meta_ptr<int>` will typically have 2 meta bits and `meta_ptr<double>` will typically have 3).

`meta_ptr` defines all operators that a normal pointer would have.

Additionally, there are functions to access the meta data, including accessing individual bits.

