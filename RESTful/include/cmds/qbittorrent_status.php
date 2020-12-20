<?php
// *** 请注意文件字符集应为 UTF-8 ***
// echo __FILE__, "#", __LINE__, "<br/>\n"; // 调试语句
//
// ----------------------------------------------------------------------------

function qbittorrent_status()
{
    // 引入全局变量，此处不要修改
    global $request;
    global $respond;

    // 这里的 qbittorrent 要做成 service !!!???
    exec("pgrep qbittorrent", $output, $return);
    $respond->err = ERR_SUCCEED;
    $respond->results->status = $return > 0 ? 1 : 0;
    return true;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 以下为内置的测试代码，可以删除或修改：
function qbittorrent_status__tunit__()
{
    if ( ! defined("APPID") )
    {
        $title = "正在执行接口测试 cmd: qbittorrent_status ...";

        require_once(dirname(__FILE__) . "/../iunit_common.php");

        $cmdObject = (object)["cmd"=>"qbittorrent_status"
                     ,"token"=>COMMON_TOKEN // 登录后返回的 token 值
                     ];

        $json = testcase($title, $cmdObject);
        normal_result($cmdObject->cmd, $json);
    }
}

// 执行测试代码
qbittorrent_status__tunit__();

// End of 测试代码
// ----------------------------------------------------------------------------
?>
