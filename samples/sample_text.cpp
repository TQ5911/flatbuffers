/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "monster_generated.h"  // Already includes "flatbuffers/flatbuffers.h".

using namespace MyGame::Sample;

// 这是一个将json文本直接解析到缓冲区，然后从缓冲区生成flatbuffer (JSON)文本的示例。
int main(int /*argc*/, const char * /*argv*/[]) {
  std::string schemafile;
  std::string jsonfile;
  // 加载 FlatBuffer schema (.fbs),读取文件内容
  bool ok = flatbuffers::LoadFile("E:\\Study\\flatbuffers\\build\\Debug\\monster_copy.fbs", false, &schemafile) &&
  // 再加载json文本,相当于读取数据
            flatbuffers::LoadFile("E:\\Study\\flatbuffers\\build\\Debug\\monsterdata.json", false, &jsonfile);
  if (!ok) {
    printf("couldn't load files!\n");
    return 1;
  }

  // 先解析模式,这个相当于是通过解析.fbs的内容,找到对应的已经定义的结构monster,然后缓冲区中创建该对象
  // 然后用它来解析数据,填充到缓冲区对象中
  flatbuffers::Parser parser;
  //const char *include_directories[] = { nullptr, nullptr };
  ok = parser.Parse(schemafile.c_str()/*, include_directories*/) &&
       parser.Parse(jsonfile.c_str()/*, include_directories*/);
  assert(ok);

  // builder_包含一个二进制缓冲区，它是解析后的数据。

  // to ensure it is correct, we now generate text back from the binary,
  // and compare the two:
  std::string jsongen;
  if (GenText(parser, parser.builder_.GetBufferPointer(), &jsongen)) {
    printf("Couldn't serialize parsed data to JSON!\n");
    return 1;
  }

  if (jsongen != jsonfile) {
    printf("%s----------------\n%s", jsongen.c_str(), jsonfile.c_str());
  }

  printf("The FlatBuffer has been parsed from JSON successfully.\n");
}
