<?php
// *** 请注意文件字符集应为 UTF-8 ***
// echo __FILE__, "#", __LINE__, "<br/>\n"; // 调试语句
//
// ----------------------------------------------------------------------------

function qbittorrent_stop()
{
    // 引入全局变量，此处不要修改
    global $request;
    global $respond;

    // 引入全局变量，此处不要修改
    global $request;
    global $respond;

    // 这里的 qbittorrent 要做成 service !!!???
    $commamd = 'sudo service qbittorrent stop';
    exec($commamd);
    exec("pgrep qbittorrent", $output, $return);
    if ( $return <= 0 )
    {
        $respond->err = ERR_SUCCEED;
    }else{
        $respond->errmsg = "停止失败";
    }
    return true;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 以下为内置的测试代码，可以删除或修改：
function qbittorrent_stop__tunit__()
{
    if ( ! defined("APPID") )
    {
        $title = "正在执行接口测试 cmd: qbittorrent_stop ...";

        require_once(dirname(__FILE__) . "/../iunit_common.php");


        $cmdObject = (object)["cmd"=>"qbittorrent_stop"
                     ,"token"=>COMMON_TOKEN // 登录后返回的 token 值
                     ];

        $json = testcase($title, $cmdObject);
        normal_result($cmdObject->cmd, $json);
    }
}

// 执行测试代码
qbittorrent_stop__tunit__();

// End of 测试代码
// ----------------------------------------------------------------------------
?>
