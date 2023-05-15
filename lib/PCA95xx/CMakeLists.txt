add_library(PCA95xx PCA95xx.h)
target_link_libraries(PCA95xx PUBLIC BitBang_I2C)
target_include_directories(PCA95xx INTERFACE .)
target_include_directories(PCA95xx PUBLIC
BitBang_I2C
)
