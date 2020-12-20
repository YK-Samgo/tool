<?php
    // 注意文件字符集统一为 UTF-8
    //echo __FILE__, "#", __LINE__, "<br/>\n"; // 调试语句

    ini_set("display_errors", "On");
    date_default_timezone_set("UTC");
	error_reporting(0);

    //header("Content-type: application/json; charset=utf-8");
    require_once("common.php");

    //测试用，有安全隐患
    header("Access-Control-Allow-Origin: *");
    header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

    $token = "";
    $respond = (object) array("err"=>ERR_FAILED);

    if ( $_SERVER["REQUEST_METHOD"] == "POST" )
    {
        $postdata = file_get_contents("php://input");
        if ( ! empty($postdata) )
        {
            $request = json_decode($postdata);
            if ( isset($request) && (gettype($request) == "array" || gettype($request) == "object") )
            {
                if ( isset($request->token) && strlen($request->token) >0 )
                {// 恢复存在的 Session
                    $token = $request->token;
                    session_id($request->token);
                    session_start();  //php5
                    //session_start(["read_and_close"=>true]);   //php7
                }else
                {// 创建新的 Session
                    //session_destroy();
                    session_start();
                    $token = session_id();
                }
                if ( isset($request->cmd) && gettype($request->cmd) == "string" )
                {
                    try {
                        include_once "cmds/" . $request->cmd . ".php";
                        if ( call_user_func($request->cmd) )
                        {

                        }else
                        {
                            errorSystem();
                        }
                    } catch(Exception $err) {
                        $respond->err    = ERR_SYSTEM;
                        $respond->errmsg = $err->getMessage();
                    }
                }else
                {
                    errorParams();
                }
            }else
            {
                errorParams();
            }
        }else
        {
            $respond->err    = ERR_PARAMS;
            $respond->errmsg = "The lack of instructions and parameters.";
        }
    }else
    if ( $_SERVER["REQUEST_METHOD"] == "GET" )
    {
        if ( isset($_GET['cno']) )
        {
            try {
                include_once("methods.php");

                $request = $_GET;

                methods((int)$_GET['cno']);
            } catch(Exception $err) {
                $respond->err    = ERR_SYSTEM;
                $respond->errmsg = $err->getMessage();
            }
        }else
        {
            $respond->protocol = "FriendSafe.i";

            $postdata = '';
            $respond->err    = ERR_SYSTEM;
            $respond->errmsg = "POST only.";
        }
    }else
    {
        $respond->protocol = "FriendSafe.i";

        $postdata = '';
        $respond->err    = ERR_SYSTEM;
        $respond->errmsg = "POST only.";
    }

    echo json_encode($respond, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT);
?>
