//
//  test_funcs.hpp
//  tacticsclone
//
//  Created by asdfuiop on 7/28/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef test_funcs_hpp
#define test_funcs_hpp

void checkthis(std::vector<std::vector<float>>& in, int rows, int cols, float& min, float & max);

void write_polygon_index_map(std::unordered_map<float, std::unordered_map<int, std::vector<int>>> into);

#endif /* test_funcs_hpp */
