#include <QApplication>
#include <QSettings>
#include <QDebug>

#include <QWidget>
#include "mainui.h"
#include "glwidget.h"


#include "cJSON/cJSON.h"

/* ========== 1. 解析测试 ========== */
static void testParse()
{
    qDebug() << "===== testParse =====";

    const char *json =
        "{"
        "  \"name\": \"Qt4Device\","
        "  \"version\": 4,"
        "  \"enabled\": true,"
        "  \"tags\": [\"arm\",\"embedded\",\"qt4\"],"
        "  \"config\": {"
        "    \"baudrate\": 115200,"
        "    \"ip\": \"192.168.1.10\""
        "  }"
        "}";

    cJSON *root = cJSON_Parse(json);
    if (!root) {
        qDebug() << "JSON parse error!";
        return;
    }

    cJSON *name    = cJSON_GetObjectItem(root, "name");
    cJSON *version = cJSON_GetObjectItem(root, "version");
    cJSON *enabled = cJSON_GetObjectItem(root, "enabled");
    cJSON *tags    = cJSON_GetObjectItem(root, "tags");
    cJSON *config  = cJSON_GetObjectItem(root, "config");

    if (name)    qDebug() << "name:" << name->valuestring;
    if (version) qDebug() << "version:" << version->valueint;
    if (enabled) qDebug() << "enabled:" << (enabled->valueint ? "true" : "false");

    /* 数组 */
    if (tags && cJSON_IsArray(tags)) {
        qDebug() << "tags:";
        for (int i = 0; i < cJSON_GetArraySize(tags); i++) {
            cJSON *t = cJSON_GetArrayItem(tags, i);
            if (t) qDebug() << "  -" << t->valuestring;
        }
    }

    /* 嵌套对象 */
    if (config) {
        cJSON *baud = cJSON_GetObjectItem(config, "baudrate");
        cJSON *ip   = cJSON_GetObjectItem(config, "ip");
        if (baud) qDebug() << "baudrate:" << baud->valueint;
        if (ip)   qDebug() << "ip:" << ip->valuestring;
    }

    cJSON_Delete(root);
}

/* ========== 2. 构造测试 ========== */
static void testCreate()
{
    qDebug() << "\n===== testCreate =====";

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "device", "ARM-Board");
    cJSON_AddNumberToObject(root, "cpu", 4);
    cJSON_AddBoolToObject(root, "online", 1);

    /* 数组 */
    cJSON *arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateString("uart"));
    cJSON_AddItemToArray(arr, cJSON_CreateString("spi"));
    cJSON_AddItemToArray(arr, cJSON_CreateString("i2c"));
    cJSON_AddItemToObject(root, "interfaces", arr);

    /* 嵌套对象 */
    cJSON *net = cJSON_CreateObject();
    cJSON_AddStringToObject(net, "ip", "10.0.0.5");
    cJSON_AddNumberToObject(net, "port", 8080);
    cJSON_AddItemToObject(root, "network", net);

    /* 输出 */
    char *out = cJSON_PrintUnformatted(root);
    qDebug() << "generated json:" << out;
    free(out);

    cJSON_Delete(root);
}

/* ========== 3. 修改已有 JSON 测试 ========== */
static void testModify()
{
    qDebug() << "\n===== testModify =====";

    const char *json = "{\"count\":1,\"msg\":\"hello\"}";
    cJSON *root = cJSON_Parse(json);

    /* 改字段 */
    cJSON *count = cJSON_GetObjectItem(root, "count");
    if (count) {
        cJSON_SetIntValue(count, 99);
    }

    /* 加字段 */
    cJSON_AddStringToObject(root, "status", "ok");

    char *out = cJSON_PrintUnformatted(root);
    qDebug() << "modified json:" << out;
    free(out);

    cJSON_Delete(root);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);



    testParse();
    testCreate();
    testModify();

//    MainUI w;
//    w.resize(800,600);
//    w.show();

//    QWidget w;
//    w.show();


//    GLWidget w;
//    w.show();
    return app.exec();
}







