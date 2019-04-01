# Baremetal Improved

- All files that include Vivado headers should be in `src-vivado/module/src`.  
  This includes header files.
- Header files that expose the public API of files in the 
  `src-vivado/module/src` folder should be in the `src-vivado/module/include`
  folder. They should not include any Vivado headers.
- All files that can be tested and run without Vivado should be in the `src` 
  folder. Header files that expose the public API of these files should be in
  `src/module/include`. These files are then included by the tests, and by 
  Vivado. Implementation files and private headers should be in 
  `src/module/src`.
- When adding a new module or functionality, write unit tests and put them in 
  the `src/module/test` folder. Update the appropriate `CMakeLists.txt` files
  to include the tests.