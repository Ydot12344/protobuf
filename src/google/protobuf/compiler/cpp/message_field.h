// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#ifndef GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_FIELD_H__
#define GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_FIELD_H__

#include <string>

#include "google/protobuf/compiler/cpp/field.h"
#include "google/protobuf/compiler/cpp/helpers.h"

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

class MessageFieldGenerator : public FieldGenerator {
 public:
  MessageFieldGenerator(const FieldDescriptor* descriptor,
                        const Options& options,
                        MessageSCCAnalyzer* scc_analyzer);
  MessageFieldGenerator(const MessageFieldGenerator&) = delete;
  MessageFieldGenerator& operator=(const MessageFieldGenerator&) = delete;
  ~MessageFieldGenerator() override;

  // implements FieldGenerator ---------------------------------------
  void GeneratePrivateMembers(io::Printer* printer) const override;
  void GenerateAccessorDeclarations(io::Printer* printer) const override;
  void GenerateInlineAccessorDefinitions(io::Printer* printer) const override;
  void GenerateNonInlineAccessorDefinitions(
      io::Printer* printer) const override;
  void GenerateInternalAccessorDeclarations(
      io::Printer* printer) const override;
  void GenerateInternalAccessorDefinitions(io::Printer* printer) const override;
  void GenerateClearingCode(io::Printer* printer) const override;
  void GenerateMessageClearingCode(io::Printer* printer) const override;
  void GenerateMergingCode(io::Printer* printer) const override;
  void GenerateSwappingCode(io::Printer* printer) const override;
  void GenerateDestructorCode(io::Printer* printer) const override;
  void GenerateConstructorCode(io::Printer* printer) const override {}
  void GenerateCopyConstructorCode(io::Printer* printer) const override;
  void GenerateSerializeWithCachedSizesToArray(
      io::Printer* printer) const override;
  void GenerateByteSize(io::Printer* printer) const override;
  void GenerateIsInitialized(io::Printer* printer) const override;
  void GenerateConstexprAggregateInitializer(
      io::Printer* printer) const override;
  void GenerateAggregateInitializer(io::Printer* printer) const override;
  void GenerateCopyAggregateInitializer(io::Printer* printer) const override;

 protected:
  const bool implicit_weak_field_;
  const bool lazy_pack_field;
  const bool has_required_fields_;
};

class MessageOneofFieldGenerator : public MessageFieldGenerator {
 public:
  MessageOneofFieldGenerator(const FieldDescriptor* descriptor,
                             const Options& options,
                             MessageSCCAnalyzer* scc_analyzer);
  MessageOneofFieldGenerator(const MessageOneofFieldGenerator&) = delete;
  MessageOneofFieldGenerator& operator=(const MessageOneofFieldGenerator&) =
      delete;
  ~MessageOneofFieldGenerator() override;

  // implements FieldGenerator ---------------------------------------
  void GenerateInlineAccessorDefinitions(io::Printer* printer) const override;
  void GenerateNonInlineAccessorDefinitions(
      io::Printer* printer) const override;
  void GenerateClearingCode(io::Printer* printer) const override;

  // MessageFieldGenerator, from which we inherit, overrides this so we need to
  // override it as well.
  void GenerateMessageClearingCode(io::Printer* printer) const override;
  void GenerateSwappingCode(io::Printer* printer) const override;
  void GenerateDestructorCode(io::Printer* printer) const override;
  void GenerateConstructorCode(io::Printer* printer) const override;
  void GenerateIsInitialized(io::Printer* printer) const override;
};

class RepeatedMessageFieldGenerator : public FieldGenerator {
 public:
  RepeatedMessageFieldGenerator(const FieldDescriptor* descriptor,
                                const Options& options,
                                MessageSCCAnalyzer* scc_analyzer);
  RepeatedMessageFieldGenerator(const RepeatedMessageFieldGenerator&) = delete;
  RepeatedMessageFieldGenerator& operator=(
      const RepeatedMessageFieldGenerator&) = delete;
  ~RepeatedMessageFieldGenerator() override;

  // implements FieldGenerator ---------------------------------------
  void GeneratePrivateMembers(io::Printer* printer) const override;
  void GenerateAccessorDeclarations(io::Printer* printer) const override;
  void GenerateInlineAccessorDefinitions(io::Printer* printer) const override;
  void GenerateClearingCode(io::Printer* printer) const override;
  void GenerateMergingCode(io::Printer* printer) const override;
  void GenerateSwappingCode(io::Printer* printer) const override;
  void GenerateConstructorCode(io::Printer* printer) const override;
  void GenerateCopyConstructorCode(io::Printer* printer) const override {}
  void GenerateDestructorCode(io::Printer* printer) const override;
  void GenerateSerializeWithCachedSizesToArray(
      io::Printer* printer) const override;
  void GenerateByteSize(io::Printer* printer) const override;
  void GenerateIsInitialized(io::Printer* printer) const override;

 private:
  const bool implicit_weak_field_;
  const bool has_required_fields_;
};

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google

#endif  // GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_FIELD_H__
