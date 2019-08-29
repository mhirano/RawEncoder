## RawVideoEncoder

### About
Encode all `.raw` videos reside in the directory `/path_to_the_data_dir` to `.avi` videos.  
Already encoded videos are automatically detected and skipped in the subsequent encoding.  

You can specify `frameSkip` − encode one image per `frameSkip` images.
`frameSkip` automatically increases when video size is too large to load on the memory. 
(Maximum memory allocation size is set to `_MAX_MEMORY_ALLOCATION_SIZE`.)
 
Also provides a utility class for `.raw` videos.
 
### Requirement
- C++17 for std::filesystem  
Checked compilers: g++-9(Mac), g++-8(Ubuntu)
- OpenCV  
Checked version: 3.4.1, 4.1.1
- OpenMP (Optional)  
You can parallelize encoding by OpenMP.
 
### Usage
You need to specify width and height of raw videos.  
You can also specify `encodingFrameRate` − frame rate of output video (default = 10)
and `frameSkip` (default = 10).  

    Usage: ./RawEncoder [/path_to_the_data_dir] [frameWidth] [frameHeight] [(Option:) encodingFrameRate (default=10)] [(Option:) frameSkip (default=10)]  
    Example: ./RawEncoder /path_to_the_data_dir 800 600  
    Example: ./RawEncoder /path_to_the_data_dir 800 600 10  
    Example: ./RawEncoder /path_to_the_data_dir 800 600 15 15  
 
### Contact
Masahiro Hirano <masahiro.dll_at_gmail.com>
    