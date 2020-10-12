#include "../../include/controllers/test-controller.hpp"

std::shared_ptr<TestController> TestController::getInstance() {
    if (instance.get() == nullptr) {
        instance = std::shared_ptr<TestController>(new TestController{});
        instance->test_service = TestService::getInstance();
    }
    return instance;
}

inline static std::regex get_test_database{"^\\/test\\/database\\/\\w+\\/\\d+$"};
inline static std::regex get_test_filesystem_namefile{"^\\/test\\/filesystem\\/[\\w=+]+\\/\\d+$"};
inline static std::regex get_test_filesystem_chunkid{"^\\/test\\/filesystem\\/[\\w=+]+\\/\\d+\\/\\d+$"};


const http::server::reply
TestController::handle(const http::server::request &req) {
    Subject sub = JWT::validateToken(req);
    if (req.method == "GET") {
        std::smatch match;
        if (std::regex_search(req.uri.begin(), req.uri.end(), match, get_test_database)) {
            GetTestDatabaseDTO get_test_database{sub};
            get_test_database.fill(req.uri);
            json list = get_test_database_table_name(get_test_database);
            return MakeReply::make_1line_dump_jsonReply<json>(
                    list, http::server::reply::ok);
        } else if (std::regex_search(req.uri.begin(), req.uri.end(), match, get_test_filesystem_namefile)) {
            GetTestFilesystemFilenameDTO get_test_filesystem_filename_dto{sub};
            get_test_filesystem_filename_dto.fill(req.uri);
            json list = get_test_filesystem_filename(get_test_filesystem_filename_dto);
            return MakeReply::make_1line_dump_jsonReply<json>(
                    list, http::server::reply::ok);

        } else if (std::regex_search(req.uri.begin(), req.uri.end(), match, get_test_filesystem_chunkid)) {

        }
    } else if (req.method == "DELETE") {
        delete_them_all();
        return http::server::reply::stock_reply(http::server::reply::ok);
    }
    throw WrongRquestFormat();

}

void TestController::delete_them_all() {
    test_service->cleanDB();
}

const json
TestController::get_test_database_table_name(const GetTestDatabaseDTO &get_test_database) {
    return test_service->getTestDatabaseTable(get_test_database);
}

const json
TestController::get_test_filesystem_filename(const GetTestFilesystemFilenameDTO &get_test_filesystem_filename_dto) {
    return test_service->getTestFilesystemFilename(get_test_filesystem_filename_dto);
}