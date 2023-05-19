#include "google/protobuf/lazy_packed_field_test.pb.h"
#include "google/protobuf/lazy_packed_field.h"

#include <gtest/gtest.h>

namespace google {
namespace protobuf {
namespace {

// Test using TLazuField<T> as container
// without parsing protobuf with lazy fields
TEST(LazyTest, LazyFieldAsContainer) {
    protobuf_unittest::File tmp;
    tmp.set_name("abacaba");
    tmp.set_extension("exe");
    tmp.set_path("/home/andrei/my_exes/");
    tmp.set_bytesize(1024);
    TLazyField<protobuf_unittest::File> lazy_file;
    {
        TLazyField<protobuf_unittest::File> tmp_lazy_file;

        ASSERT_TRUE(tmp_lazy_file.ParseFromString(tmp.SerializeAsString()));
        lazy_file = tmp_lazy_file;

        ASSERT_EQ(tmp_lazy_file.SerializeAsString(), tmp.SerializeAsString());
        ASSERT_EQ(tmp_lazy_file.Unpack()->SerializeAsString(), tmp.SerializeAsString());
    }

    ASSERT_EQ(lazy_file.SerializeAsString(), tmp.SerializeAsString());
    ASSERT_EQ(lazy_file.Unpack()->SerializeAsString(), tmp.SerializeAsString());
}

// Test using TLazuField<T> while parsing/copying
// messages with lazy fields
TEST(LazyTest, ParseProtoWithLazyField) {
    protobuf_unittest::Folder folder;
    protobuf_unittest::FolderLazy lazy_folder;

    folder.set_name("my_folder");
    folder.set_path("/home/andrei");

    size_t folder_size = 0;
    
    auto* file = folder.add_files();
    file->set_name("file1");
    file->set_extension("exe");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(512);
    folder_size += 512;

    file = folder.add_files();
    file->set_name("file2");
    file->set_extension("txt");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(1024);
    folder_size += 1024;

    file = folder.add_files();
    file->set_name("file3");
    file->set_extension("cpp");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(2048);
    folder_size += 2048;

    folder.set_bytesize(folder_size);

    std::string expected_bin = folder.SerializeAsString();

    ASSERT_TRUE(lazy_folder.ParseFromString(expected_bin));
    ASSERT_EQ(lazy_folder.SerializeAsString(), expected_bin);

    ASSERT_EQ(lazy_folder.files().size(), 3);
    ASSERT_EQ(lazy_folder.files()[0].Unpack()->name(), "file1");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->name(), "file2");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->name(), "file3");

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->path(), "/home/andrei/my_folder");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->path(), "/home/andrei/my_folder");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->path(), "/home/andrei/my_folder");

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->bytesize(), 512);
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->bytesize(), 1024);
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->bytesize(), 2048);

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->extension(), "exe");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->extension(), "txt");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->extension(), "cpp");

    protobuf_unittest::FolderLazy another_lazy_folder;
    *another_lazy_folder.add_files() = lazy_folder.files(0);
    *another_lazy_folder.add_files() = lazy_folder.files(2);

    ASSERT_TRUE(folder.ParseFromString(another_lazy_folder.SerializeAsString()));
    ASSERT_EQ(folder.SerializeAsString(), another_lazy_folder.SerializeAsString());

    ASSERT_EQ(folder.files().size(), 2);
    
    ASSERT_EQ(folder.files(0).SerializeAsString(), another_lazy_folder.files(0).SerializeAsString());
    ASSERT_EQ(folder.files(1).SerializeAsString(), another_lazy_folder.files(1).SerializeAsString());

    ASSERT_EQ(folder.files(0).name(), lazy_folder.files(0).Unpack()->name());
    ASSERT_EQ(folder.files(0).extension(), lazy_folder.files(0).Unpack()->extension());
    ASSERT_EQ(folder.files(0).path(), lazy_folder.files(0).Unpack()->path());
    ASSERT_EQ(folder.files(0).bytesize(), lazy_folder.files(0).Unpack()->bytesize());

    ASSERT_EQ(folder.files(1).name(), lazy_folder.files(2).Unpack()->name());
    ASSERT_EQ(folder.files(1).extension(), lazy_folder.files(2).Unpack()->extension());
    ASSERT_EQ(folder.files(1).path(), lazy_folder.files(2).Unpack()->path());
    ASSERT_EQ(folder.files(1).bytesize(), lazy_folder.files(2).Unpack()->bytesize());
}

// Like ParseProtoWithLazyField but with stream parsing
TEST(LazyTest, ParseProtoWithLazyFieldFromStream) {
    protobuf_unittest::Folder folder;
    protobuf_unittest::FolderLazy lazy_folder;

    folder.set_name("my_folder");
    folder.set_path("/home/andrei");

    size_t folder_size = 0;
    
    auto* file = folder.add_files();
    file->set_name("file1");
    file->set_extension("exe");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(512);
    folder_size += 512;

    file = folder.add_files();
    file->set_name("file2");
    file->set_extension("txt");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(1024);
    folder_size += 1024;

    file = folder.add_files();
    file->set_name("file3");
    file->set_extension("cpp");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(2048);
    folder_size += 2048;

    folder.set_bytesize(folder_size);

    std::string expected_bin = folder.SerializeAsString();
    std::stringstream ss(expected_bin);

    ASSERT_TRUE(lazy_folder.ParseFromIstream(&ss));
    ASSERT_EQ(lazy_folder.SerializeAsString(), expected_bin);

    ASSERT_EQ(lazy_folder.files().size(), 3);
    ASSERT_EQ(lazy_folder.files()[0].Unpack()->name(), "file1");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->name(), "file2");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->name(), "file3");

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->path(), "/home/andrei/my_folder");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->path(), "/home/andrei/my_folder");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->path(), "/home/andrei/my_folder");

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->bytesize(), 512);
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->bytesize(), 1024);
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->bytesize(), 2048);

    ASSERT_EQ(lazy_folder.files()[0].Unpack()->extension(), "exe");
    ASSERT_EQ(lazy_folder.files()[1].Unpack()->extension(), "txt");
    ASSERT_EQ(lazy_folder.files()[2].Unpack()->extension(), "cpp");

    protobuf_unittest::FolderLazy another_lazy_folder;
    *another_lazy_folder.add_files() = lazy_folder.files(0);
    *another_lazy_folder.add_files() = lazy_folder.files(2);

    ss = std::stringstream(another_lazy_folder.SerializeAsString());
    ASSERT_TRUE(folder.ParseFromIstream(&ss));
    ASSERT_EQ(folder.SerializeAsString(), another_lazy_folder.SerializeAsString());

    ASSERT_EQ(folder.files().size(), 2);
    
    ASSERT_EQ(folder.files(0).SerializeAsString(), another_lazy_folder.files(0).SerializeAsString());
    ASSERT_EQ(folder.files(1).SerializeAsString(), another_lazy_folder.files(1).SerializeAsString());

    ASSERT_EQ(folder.files(0).name(), lazy_folder.files(0).Unpack()->name());
    ASSERT_EQ(folder.files(0).extension(), lazy_folder.files(0).Unpack()->extension());
    ASSERT_EQ(folder.files(0).path(), lazy_folder.files(0).Unpack()->path());
    ASSERT_EQ(folder.files(0).bytesize(), lazy_folder.files(0).Unpack()->bytesize());

    ASSERT_EQ(folder.files(1).name(), lazy_folder.files(2).Unpack()->name());
    ASSERT_EQ(folder.files(1).extension(), lazy_folder.files(2).Unpack()->extension());
    ASSERT_EQ(folder.files(1).path(), lazy_folder.files(2).Unpack()->path());
    ASSERT_EQ(folder.files(1).bytesize(), lazy_folder.files(2).Unpack()->bytesize());
}

TEST(LazyTest, TestLazyChangeUnpack) {
    protobuf_unittest::Folder folder;
    protobuf_unittest::FolderLazy lazy_folder;

    folder.set_name("my_folder");
    folder.set_path("/home/andrei");

    size_t folder_size = 0;
    
    auto* file = folder.add_files();
    file->set_name("file1");
    file->set_extension("exe");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(512);
    folder_size += 512;

    file = folder.add_files();
    file->set_name("file2");
    file->set_extension("txt");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(1024);
    folder_size += 1024;

    file = folder.add_files();
    file->set_name("file3");
    file->set_extension("cpp");
    file->set_path("/home/andrei/my_folder");
    file->set_bytesize(2048);
    folder_size += 2048;

    folder.set_bytesize(folder_size);

    ASSERT_TRUE(lazy_folder.ParseFromString(folder.SerializeAsString()));

    for (size_t i = 0; i < lazy_folder.files().size(); i++) {
        lazy_folder.files(i).Unpack()->set_name("new_file" + std::to_string(i));
    }

    ASSERT_TRUE(folder.ParseFromString(lazy_folder.SerializeAsString()));

    for (size_t i = 0; i < folder.files().size(); i++) {
        ASSERT_EQ(folder.files(i).name(), "new_file" + std::to_string(i));
    }
}

std::string GenStr(size_t size, char start) {
    std::string res;
    res.reserve(size);

    for (size_t i = 0; i < size; i++) {
        res += static_cast<char>((start + i) % 26) + 'a';
    }

    return res;
}

TEST(LazyTest, TestLazyBigProto) {
    protobuf_unittest::BigProto big_proto;
    protobuf_unittest::BigProtoLazy big_proto_lazy;

    const size_t LEN = 10'000'000;
    const size_t CNT_FILES = 100;

    std::string start_data_expected;
    std::string end_data_expected;
    start_data_expected.reserve(LEN);
    end_data_expected.reserve(LEN);

    start_data_expected = GenStr(LEN, 'a');
    end_data_expected = GenStr(LEN, 'e');
    big_proto.set_start_data(start_data_expected);
    big_proto.set_end_data(end_data_expected);

    for (size_t i = 0; i < CNT_FILES; i++) {
        auto* file = big_proto.mutable_folder()->add_files();
        file->set_name(GenStr(LEN / 1000, static_cast<char>(('a' + i) % 26 + 'a')));
        file->set_path(GenStr(LEN / 1000, static_cast<char>(('c' + i) % 26 + 'a')));
    }

    std::string big_proto_bin = big_proto.SerializeAsString();
    ASSERT_TRUE(big_proto_lazy.ParseFromString(big_proto_bin));

    ASSERT_EQ(big_proto_lazy.start_data(), start_data_expected);
    ASSERT_EQ(big_proto_lazy.end_data(), end_data_expected);
    ASSERT_EQ(big_proto_lazy.folder().Unpack()->files().size(), CNT_FILES);

    auto* folder_ = big_proto_lazy.folder().Unpack();
    for (size_t i = 0; i < folder_->files().size(); i++) {
        ASSERT_EQ(folder_->files(i).name(), GenStr(LEN / 1000, static_cast<char>(('a' + i) % 26 + 'a')));
        ASSERT_EQ(folder_->files(i).path(), GenStr(LEN / 1000, static_cast<char>(('c' + i) % 26 + 'a')));
    }

    std::stringstream ss(big_proto_bin);
    ASSERT_TRUE(big_proto_lazy.ParseFromIstream(&ss));

    ASSERT_EQ(big_proto_lazy.start_data(), start_data_expected);
    ASSERT_EQ(big_proto_lazy.end_data(), end_data_expected);
    ASSERT_EQ(big_proto_lazy.folder().Unpack()->files().size(), CNT_FILES);

    folder_ = big_proto_lazy.folder().Unpack();
    for (size_t i = 0; i < folder_->files().size(); i++) {
        ASSERT_EQ(folder_->files(i).name(), GenStr(LEN / 1000, static_cast<char>(('a' + i) % 26 + 'a')));
        ASSERT_EQ(folder_->files(i).path(), GenStr(LEN / 1000, static_cast<char>(('c' + i) % 26 + 'a')));
    }
}

TEST(LazyTest, TestLazyWithNestedLazy) {
    protobuf_unittest::FolderWraper wraper;
    protobuf_unittest::LazyFolderLazyWraper lazy_wraper;

    const size_t LEN = 1000;
    const size_t CNT = 1000;

    std::string start_data_expected = GenStr(LEN, 'a');
    std::string end_data_expected = GenStr(LEN, 'y');
    wraper.set_start_data(start_data_expected);
    wraper.set_end_data(end_data_expected);

    wraper.mutable_folder()->set_name(GenStr(LEN, 'z'));
    wraper.mutable_folder()->set_path(GenStr(LEN, 'i'));

    for (size_t i = 0; i < CNT; i++) {
        auto* file = wraper.mutable_folder()->add_files();
        file->set_name(GenStr(LEN / 1000, static_cast<char>(('a' + i) % 26 + 'a')));
        file->set_path(GenStr(LEN / 1000, static_cast<char>(('c' + i) % 26 + 'a')));
    }

    ASSERT_TRUE(lazy_wraper.ParseFromString(wraper.SerializeAsString()));
    ASSERT_EQ(lazy_wraper.start_data(), start_data_expected);
    ASSERT_EQ(lazy_wraper.end_data(), end_data_expected);

    auto* folder_ = lazy_wraper.folder().Unpack();

    ASSERT_EQ(folder_->name(), GenStr(LEN, 'z'));
    ASSERT_EQ(folder_->path(), GenStr(LEN, 'i'));
    ASSERT_EQ(folder_->files().size(), CNT);

    for (size_t i = 0; i < folder_->files().size(); i++) {
        auto* file = folder_->files(i).Unpack();
        ASSERT_EQ(file->name(), GenStr(LEN / 1000, static_cast<char>(('a' + i) % 26 + 'a')));
        ASSERT_EQ(file->path(), GenStr(LEN / 1000, static_cast<char>(('c' + i) % 26 + 'a')));
    }
}

}  // namespace
}  // namespace protobuf
}  // namespace google