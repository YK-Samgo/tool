# qBittorrent 内置服务器接口

## 注意本文目前只是范例，内容需要根据实际需要进行调整


基于FSI 2.1的接口协议规范，采用 http/https方式传递

### FSI v2.1 的基本协议格式
本协议相关的 FSI v2.1 协议规范提要：
* 请求参数和返回参数统一采用标准 JSON 格式
* 统一采用 utf8 字符集 
* 所有自身主键全部为小写英数和下划线组成，且不得以数字开头

**请求参数格式**

    {
        "cmd": "指令名" // 指令名全部为小写英数和下划线组成，且不得以数字开头
        "token": "token值" // 登录成功后返回的 token值应当包括在后续所有接口请求中
        "params": { // 参数表。没有参数时，允许不存在此键值
            ... ... // 实际参数根据业务需求确定
        }
    }
如果没有参数，可以省略整个 params {}

**返回参数格式**

    {
        "err": 0 // 错误码，0 表示没有错误
        "results": { // 返回内容。没有返回内容时，可能不存在此键值
            ... ... // 实际内容根据业务需求确定

            "records": [ // 若返回数据集，则存在该数组中，元素为每个数据对象
                {
                    ... ... // 实际内容根据业务需求确定
                },
                ... ...
            ]
        }
    }
返回后先判断err是否为0，然后读取 results 中的返回内容。

### 接口地址：
当前应用的 ./include/index.php ，此为 U-nas 标准

### 服务功能列表

#### 启动 qBittorrent 服务

**请求参数**

    {
        "port": 8080
    }

**返回参数**

    {
        "err": 0 // 错误码，0 表示没有错误
    }

**错误返回**
可能出现的错误原因：
| err | 说明 |
| - | - |
| 9999 | 启动失败，具体原因不明 | 
---

#### 停止 qBittorrent 服务

**请求参数**

    # 没有参数
    {
    }

**返回参数**

    {
        "err": 0 // 错误码，0 表示没有错误
    }

**错误返回**
可能出现的错误原因：
| err | 说明 |
| - | - |
| 9999 | 停止失败，具体原因不明 | 
---

#### 查看 qBittorrent 服务是否已启动

**请求参数**

    # 没有参数
    {
    }

**返回参数**

    {
        "err": 0 // 错误码，0 表示没有错误
    }

**错误返回**
可能出现的错误原因：
| err | 说明 |
| - | - |

---


## 常见错误码

| 错误标识 | 错误码 | 说明 / 含义
| - | - | -
| ERR_SUCCEED       |    0 | 正确 / 正常 / 成功
| ERR_FAILED        | 9999 | 打开失败 / 操作失败 / 连接失败
| ERR_TIMEOUT       | 9998 | 操作超时
| ERR_NO_INPUT      | 9997 | 缺少必要的输入 / 必输项遗漏
| ERR_DAMAGE        | 9996 | 损坏（文件等）
| ERR_PARAMS        | 9995 | 参数内容错误
| ERR_NOT_EXISTS    | 9994 | 不存在
| ERR_EXISTS        | 9993 | 已经存在
| ERR_BUSY          | 9992 | 忙 / 操作被忽略
| ERR_LOAD          | 9991 | 读取失败 / 装载失败
| ERR_SAVE          | 9990 | 写入失败 / 保存失败
| ERR_CONTENT       | 9989 | 内容错误 / 内容无法识别
| ERR_AUTHORIZE     | 9988 | 认证失败，或权限不足
| ERR_SYSTEM        | 9987 | 系统错误
| ERR_LOCKED        | 9986 | 已经锁定
| ERR_NOT_ACTIVE    | 9985 | 尚未激活
| ERR_NOT_LOGGED_IN | 9984 | 尚未登录
| ERR_NOT_SCENE     | 9983 | 场景错误
| ERR_USED          | 9982 | 已经使用过了，或没有使用的配额了
| ERR_UNCOOLED      | 9981 | 尚未完全冷却
| ERR_MAINTENANCE   | 9980 | 维护状态，请稍候再使用
| ERR_LOGIN_OUTSIDE | 9979 | 已在别处登录
| ERR_SCRIPT_FAILED | 9978 | 脚本执行错误
| ERR_SERVER_OFFLINE | 9977 | 服务器离线或异常
| ERR_SCENE_EXISTS  | 9976 | 已经处于场景中 / 应先退出当前场景
| ERR_SQLERROR      | 9975 | 数据库操作发生错误
| ERR_INCOMPATIBLE  | 9974 | 系统不兼容或不支持的请求
| ERR_NOT_ENOUGH    | 9973 | 不足或不够
| ERR_REQUEST_FAILED | 9972 | 无效请求，或无权执行该指令
| ERR_OFFLINE       | 9971 | 离线或已断开连接
| ERR_CONFIG        | 9970 | 配置错误
| ERR_CREATE        | 9969 | 创建失败（多用于文件）
| ERR_CHANGE        | 9968 | 修改失败（多用于文件）
| ERR_DELETE        | 9967 | 删除失败（多用于文件）
| ERR_CANCEL        | 9966 | 被取消，这个不一定是错误
| ERR_UNKNOWN       | 9965 | 未知。可能不是错误
| ERR_UNTREATED     | 9964 | 未处理/未开始。可能不是错误
| ERR_CORRELATIVE   | 9963 | 存在关联性
| ERR_DIFFERENCE    | 9962 | 不同、不一致、存在差异
| ERR_VERSION       | 9961 | 版本不一致或不匹配
| ERR_READY         | 9960 | 准备状态，不一定是错误
| ERR_NO_RESPONSE   | 9959 | 没有响应
| ERR_BEHIND        | 9958 | 落后了，被人抢先了
| ERR_REDUPLICATED  | 9957 | 重复的
| ERR_PWDERR        | 9956 | 用户不存在或密码错误，或服务已经到期。
| ERR_UNCCGDPT      | 9955 | 授权目录未选择或授权对象未选择。

