<?php
    // 字符集请设置为 UTF-8

    ini_set("track_errors", true);

    define("APPID", "qbittorrent-unas");

    // 错误代码
    define("ERR_SUCCEED"       ,    0); // 正确 / 正常 / 成功
    define("ERR_FAILED"        , 9999); // 打开失败 / 操作失败 / 连接失败
    define("ERR_TIMEOUT"       , 9998); // 操作超时
    define("ERR_NO_INPUT"      , 9997); // 缺少必要的输入 / 必输项遗漏
    define("ERR_DAMAGE"        , 9996); // 损坏（文件等）
    define("ERR_PARAMS"        , 9995); // 参数内容错误
    define("ERR_NOT_EXISTS"    , 9994); // 不存在
    define("ERR_EXISTS"        , 9993); // 已经存在
    define("ERR_BUSY"          , 9992); // 忙 / 操作被忽略
    define("ERR_LOAD"          , 9991); // 读取失败 / 装载失败
    define("ERR_SAVE"          , 9990); // 写入失败 / 保存失败
    define("ERR_CONTENT"       , 9989); // 内容错误 / 内容无法识别
    define("ERR_AUTHORIZE"     , 9988); // 认证失败，或权限不足
    define("ERR_SYSTEM"        , 9987); // 系统错误
    define("ERR_LOCKED"        , 9986); // 已经锁定
    define("ERR_NOT_ACTIVE"    , 9985); // 尚未激活
    define("ERR_NOT_LOGGED_IN" , 9984); // 尚未登录
    define("ERR_NOT_SCENE"     , 9983); // 场景错误
    define("ERR_USED"          , 9982); // 已经使用过了，或没有使用的配额了
    define("ERR_UNCOOLED"      , 9981); // 尚未完全冷却
    define("ERR_MAINTENANCE"   , 9980); // 维护状态，请稍候再使用
    define("ERR_LOGIN_OUTSIDE" , 9979); // 已在别处登录
    define("ERR_SCRIPT_FAILED" , 9978); // 脚本执行错误
    define("ERR_SERVER_OFFLINE", 9977); // 服务器离线或异常
    define("ERR_SCENE_EXISTS"  , 9976); // 已经处于场景中 / 应先退出当前场景
    define("ERR_SQLERROR"      , 9975); // 数据库操作发生错误
    define("ERR_INCOMPATIBLE"  , 9974); // 系统不兼容或不支持的请求
    define("ERR_NOT_ENOUGH"    , 9973); // 不足或不够
    define("ERR_REQUEST_FAILED", 9972); // 无效请求，或无权执行该指令
    define("ERR_OFFLINE"       , 9971); // 离线或已断开连接
    define("ERR_CONFIG"        , 9970); // 配置错误
    define("ERR_CREATE"        , 9969); // 创建失败（多用于文件）
    define("ERR_CHANGE"        , 9968); // 修改失败（多用于文件）
    define("ERR_DELETE"        , 9967); // 删除失败（多用于文件）
    define("ERR_CANCEL"        , 9966); // 被取消，这个不一定是错误
    define("ERR_UNKNOWN"       , 9965); // 未知。可能不是错误
    define("ERR_UNTREATED"     , 9964); // 未处理/未开始。可能不是错误
    define("ERR_CORRELATIVE"   , 9963); // 存在关联性
    define("ERR_DIFFERENCE"    , 9962); // 不同、不一致、存在差异
    define("ERR_VERSION"       , 9961); // 版本不一致或不匹配
    define("ERR_READY"         , 9960); // 准备状态，不一定是错误
    define("ERR_NO_RESPONSE"   , 9959); // 没有响应
    define("ERR_BEHIND"        , 9958); // 落后了，被人抢先了
    define("ERR_REDUPLICATED"  , 9957); // 重复的
    define("ERR_PWDERR"        , 9956); // 用户不存在或密码错误，或服务已经到期。
    define("ERR_UNCCGDPT"      , 9955); // 授权目录未选择或授权对象未选择。

?>