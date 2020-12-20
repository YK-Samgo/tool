<?php
    // Windows 中字符集为 ANSI/GB2312
    // Linux   中字符集为 UTF-8

    require_once("settings.php");

	date_default_timezone_set('PRC');

    define("LOGIN_FOR_PLAYER", 1);

    function loggedIn()
    {
        return validInteger($_SESSION["no"]);
    }

    function playerLoggedIn()
    {
        $ret = 0;
        if ( validString($_SESSION["uid"]) )
        {// 注册用户
            $ret = 2;
        }else
        if ( validInteger($_SESSION["no"]) )
        {// 游客、匿名用户
            $ret = 1;
        }
        return $ret;
    }

    function dmsLoggedIn()
    {
        $ret = 0;
        if ( validInteger($_SESSION["dms"]) && validString($_SESSION["uid"]) )
        {// 管理平台登录用户
            if ( validInteger($_SESSION["manage"]) )
            {// 管理平台主管/经理
                $ret = 5;
            }else
            {// 普通管理人员
                $ret = 4;
            }
        }
        return $ret;
    }

    function isLoggedIn($loginType = 0)
    {
        return isset($_SESSION["no"]) && $_SESSION["no"] >0;
    }

    function oprNo()
    {
        return validInteger($_SESSION["no"]) ? $_SESSION["no"] : 0;
    }
    function app()
    {
        return validString($_SESSION["app"]) ? $_SESSION["app"] : "";
    }
    function appid()
    {
        return validString($_SESSION["appid"]) ? $_SESSION["appid"] : "";
    }
    function getAppid($app)
    {
        $pos = strpos($app, "-");
        return $pos >0 ? substr($app, 0, $pos) : $app;
    }
    function toJsonArray(&$v)
    {
        return (isset($v) && gettype($v) == "array") ?  json_encode($v) : "[]";
    }

    function isMobile()
    {
        return isset($_SESSION["app"]) && ($_SESSION["app"] === "alhz_android" || $_SESSION["app"] === "alhz_iphone")
            && validInteger($_SESSION["ver"]);
    }

    function validMobile($appid, $ver)
    {
        $res = $appid == "alhz_android"
            || $appid == "alhz_iphone";

        return $res && $ver >= 1;
    }

    function validAppID($appid, $ver)
    {
        return ($appid === "alhz_android" && $ver >=1)
            || ($appid === "alhz_iphone"  && $ver >=1)
            || ($appid === "alhz_www"     && $ver >=1)
            || ($appid === "alhz_wxgzh"   && $ver >=1)
            || ($appid === "alhz_wxxcx"   && $ver >=1);
    }

    function asval(&$v, $def="")
    {
        return !isset($v)||is_null($v) ? $def : $v;
    }
/*
    function asdate(&$v, $def="") // yyyy-MM-dd
    {
        return !isset($v)||is_null($v) ? $def : ($v < "1910-01-01" ? "" : substr($v, 0, 10));
    }
*/
    function asdatetime(&$v, $def="") // yyyy-MM-dd HH:mm:ss
    {
        return !isset($v)||is_null($v) ? $def : ($v < "1910-01-01" ? "" : substr($v, 0, 19));
    }

    function astime(&$v, $def="") // HH:mm:ss
    {
        return !isset($v)||is_null($v) ? def : substr($v, 0, 8);
    }

    function isStamp(&$stamp)
    {
        $ret = strtotime($stamp);
        return gettype($ret) === "integer" ? true : false;
    }

    function stampFormat(&$stamp, $default="1970-01-01 00:00:00")
    {
        $res = strtotime($stamp);
        return gettype($res) === "integer" ? date('Y-m-d H:i:s', $res) : $default;
    }

    function asFormat(&$date, $default="1970-01-01") // 命名不合理，弃用
    {
        $res = strtotime($date);
        return gettype($res) === "integer" ? date('Y-m-d', $res) : $default;
    }

    function asDate(&$date, $default="1970-01-01")
    {
        $res = strtotime($date);
        return gettype($res) === "integer" ? date('Y-m-d', $res) : $default;
    }

    function isinteger(&$v)
    {
        return isset($v) && gettype($v) === "integer";
    }

    function isnumeric(&$v)
    {
        return isset($v) && (gettype($v) === "integer" || gettype($v) === "double");
    }

    function isstring(&$v)
    {
        return isset($v) && gettype($v) === "string";
    }

    function validInteger(&$v)
    {
        return isset($v) && gettype($v) === "integer" && $v>0;
    }
    function validString(&$v)
    {
        return isset($v) && gettype($v)=="string" && strlen($v)>0;
    }
    function validNumeric(&$v)
    {
        return isset($v) && (gettype($v) === "integer" || gettype($v) === "double") && $v >0.0;
    }
    function zeroIf(&$v, $defvalue=0)
    {
        if ( !isset($v) || gettype($v) !== "integer" )
        {
            $v = $defvalue;
        }
        return $v;
    }
    function numericIf(&$v, $defvalue=0.0)
    {
        if ( !isset($v) || (gettype($v) !== "integer" && gettype($v) !== "double") )
        {
            $v = $defvalue;
        }
        return $v;
    }
    function emptyIf(&$v, $defvalue="")
    {
        if ( !isset($v) || gettype($v) !== "string" )
        {
            $v = $defvalue;
        }
        return $v;
    }

    function checkDateFormat($date) {
        if (! isset($date) || $date=='') {
            return true;
        }

        // match the format of the date
        if (preg_match("/^\d{4}-\d{2}-\d{2}/", $date)) {
            // check whether the date is valid of not
            if (strtotime($date)) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
    //将二维数组转换为字符串
    function arr2str ($arr)
    {
        $t="";
        foreach ($arr as $v)
        {
            $t .= $v[0].','.$v[1].':';
        }
        $newstr = substr($t,0,strlen($t)-1);
        return $newstr;
    }

    // =====================================================================================================================

    function recordSQL(&$rs, $sqlid, $params)
    {
        $ret = false;
        $pdo = new PDO(DC_CONNECTION, DC_USERNAME, DC_PASSWORD);
        if ( $pdo )
        {
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $p = "";
            foreach($params as $k=>$v)
            {
                $p .= ",?";
            }
            if ( $p != "" )
            {
                $p = substr($p, 1, strlen($p)-1);
            }
            try {
                $query = $pdo->prepare("SELECT *, (row_data).* FROM $sqlid($p);", array(PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL));
                if ( $params )
                {
                    $done = $query->execute($params);
                }else
                {
                    $done = $query->execute();
                }
                $query->setFetchMode(PDO::FETCH_ASSOC);
                $rs = $query;

                //$reccount = $query["rec_count"];

                $ret = true;
            } finally {
                unset($pdo);
            }
        }else
        {
            echo "Why???\n";
        }
        return $ret;
    }

    function execSQL(&$rs, $sqlid, $params, $hasCount = false)
    {
        $ret = false;
        $pdo = new PDO(DC_CONNECTION, DC_USERNAME, DC_PASSWORD);
        if ( $pdo )
        {
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $p = "";
            foreach($params as $k=>$v)
            {
                $p .= ",?";
            }
            if ( $p != "" )
            {
                $p = substr($p, 1, strlen($p)-1);
            }
            try {
                if ($hasCount) {
                    $query = $pdo->prepare("SELECT *, (row_data).* FROM $sqlid($p);", array(PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL));
                } else {
                    $query = $pdo->prepare("SELECT * FROM $sqlid($p);");
                }
                if ( $params )
                {
                    $done = $query->execute($params);
                }else
                {
                    $done = $query->execute();
                }
                $query->setFetchMode(PDO::FETCH_ASSOC);
                $rs = $query;

                $ret = true;
            } finally {
                unset($pdo);
            }
        }else
        {
            echo "Why???\n";
        }
        return $ret;
    }
    function debugSQL($postdata, $result, $ipaddr, $starttime)
    {
        $ret = false;
        try {
            $pdo = new PDO(DC_BUSINESS, DC_USERNAME, DC_PASSWORD);
            if ( $pdo )
            {
                $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

                $params = array($postdata, $result, $ipaddr, $starttime);
                try {
                    $query = $pdo->prepare("SELECT DEBUG(?, ?, ?, ?);");
                    $done = $query->execute($params);

                    $ret = true;
                } finally {
                    unset($pdo);
                }
            }
        } catch(PDOException $err) {
            echo "\n>>> ", $err->getMessage(), "<br>";
        }
        return $ret;
    }
    function debugSQL3($postdata, $result, $ipaddr, $starttime)
    {
        $ret = false;
        try {
            $pdo = new PDO(DC_RESOURCE, DC_USERNAME, DC_PASSWORD);
            if ( $pdo )
            {
                $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

                $params = array($postdata, $result, $ipaddr, $starttime);
                try {
                    $query = $pdo->prepare("SELECT DEBUG(?, ?, ?, ?);");
                    $done = $query->execute($params);

                    $ret = true;
                } finally {
                    unset($pdo);
                }
            }
        } catch(PDOException $err) {
            echo "\n>>> ", $err->getMessage(), "<br>";
        }
        return $ret;
    }
    function getRow(&$rs, $sqlid, $keyfield, $no)
    {
        $ret = false;
        $pdo = new PDO(DC_CONNECTION, DC_USERNAME, DC_PASSWORD);
        if ( $pdo )
        {
            $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            try {
                $query = $pdo->prepare("SELECT * FROM $sqlid WHERE $keyfield=$no;");
                $done = $query->execute();
                $query->setFetchMode(PDO::FETCH_ASSOC);
                if($query->rowCount()>0) {
                    $rs = $query->fetch();
                    $ret = true;
                }
            } finally {
                unset($pdo);
            }
        }
        return $ret;
    }
    function queryNext($rs)
    {
        return $rs->fetch();
    }
    function queryResult($rs)
    {
        $ret = 0;
        if ( $row = $rs->fetch() )
        {
            foreach($row as $k=>$v)
            {
                $ret = $v;
                break;
            }
        }
        return $ret;
    }
    function queryDataCount($rs)
    {
        $row = $rs->fetch(PDO::FETCH_ASSOC, PDO::FETCH_ORI_FIRST);
        return $row['datacount'];
    }

    function errorParams()
    {
        global $respond;
        $respond->err   = ERR_PARAMS;
        $respond->error = local8bit("缺少参数或参数内容错误。");
    }

    function errorSystem()
    {
        global $respond;
        $respond->err   = ERR_SYSTEM;
        $respond->error = local8bit("系统错误，请与开发商联系。");
    }

    function errorNotLoggedIn()  //没有uid
    {
        global $respond;
        $respond->err   = ERR_NOT_LOGGED_IN;
        $respond->error = local8bit("尚未登录。");
    }

    function errorNotAuthorized()  //没有no
    {
        global $respond;
        $respond->err   = ERR_AUTHORIZE;
        $respond->error = local8bit("请登录或注册。");
    }

    function errorNotExport()
    {
        global $respond;
        $respond->err   = ERR_NOT_SCENE;
        $respond->error = local8bit("不能导出数据。");
    }

    function errorNotImport()
    {
        global $respond;
        $respond->err   = ERR_NOT_SCENE;
        $respond->error = local8bit("不能导入数据。");
    }

    function local8bit($s)
    {
        return $s;
    }
    function dbFields($record, $field)
    {
        foreach((array)$record as $k=>$v)
        {
            if ( strcasecmp($k, $field) == 0 )
            {
                return $v;
            }
        }
        return NULL;
    }

    function random($length)
    {
        $ret = "";
        $number = "0123456789";
        $numlen = strlen($number)-1;

        for( $i=0; $i<$length; $i++)
        {
            $ret.=$number[rand(0, $numlen)];
        }

        return $ret;
    }

    function myOnlyId()
    {
       if ( function_exists('com_create_guid') )
       {
           return com_create_guid();
       }else
       {
           mt_srand((double)microtime()*10000);//optional for php 4.2.0 and up.
           return strtoupper(md5(uniqid(rand(), true)));
       }
    }

    // =====================================================================================================================

    function http_post($url, $data)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60000);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, 1);
        $ret = curl_exec($ch);
        curl_close($ch);

        return $ret;
    }

    function http_get($url)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60000);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, 0);
        $ret = curl_exec($ch);
        curl_close($ch);

        return $ret;
    }

    function https_get($url)
    {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false); // 跳过证书检查
        curl_setopt($ch, CURLOPT_HEADER, 0);
        curl_setopt($ch, CURLOPT_TIMEOUT, 60000);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_POST, 0);
        $ret = curl_exec($ch);
        curl_close($ch);

        return $ret;
    }

    function clientOS()
    {
        $agent = strtolower($_SERVER['HTTP_USER_AGENT']);
        $is_pc = (strpos($agent, 'windows nt')) ? true : false;
        $is_mac = (strpos($agent, 'mac os')) ? true : false;
        $is_iphone = (strpos($agent, 'iphone')) ? true : false;
        $is_android = (strpos($agent, 'android')) ? true : false;
        $is_ipad = (strpos($agent, 'ipad')) ? true : false;

        $ret = 0; // 未知，不可识别
        if ( $is_android )
        {
            $ret = 4;
        }else
        if ( $is_iphone )
        {
            $ret = 5;
        }else
        if ( $is_ipad )
        {
            $ret = 3;
        }else
        if ( $is_pc )
        {
            $ret = 1;
        }else
        if ( $is_mac )
        {
            $ret = 2;
        }
        return $ret;
    }

    function printValue($value, $key="", $ident=0)
    {
        $identStr = "";
        for( $i=0; $i<$ident*4; $i++ )
        {
            $identStr = $identStr . "&nbsp;";
        }

        if ( ! empty($key) )
        {
            $key = $key . "\t ";
        }

        $type = gettype($value);
        if ( $type == "string" || $type == "integer" )
        {
            echo $identStr, $key, "(", $type, ") = ", $value, "\n<br/>";
        }else
        if ( $type == "boolean" )
        {
            echo $identStr, $key, "= ", $value ? "true" : "false", "\n<br/>";
        }else
        if ( $type == "NULL" )
        {
            echo $identStr, $key, "is NULL \n<br/>";
        }else
        if ( $type == "array" )
        {
            echo $identStr, $key, ":\n<br/>";
            foreach((array)$value as $k=>$v)
            {
                printValue($v, $k, $ident+1);
            }
        }else
        {
            echo $identStr, $key, " >>> ", gettype($value), "\n<br/>";
            foreach((array)$value as $k=>$v)
            {
                printValue($v, $k, $ident+1);
            }
        }
    }

    function udate($format = 'u', $utimestamp = null) {
       if (is_null($utimestamp))
           $utimestamp = microtime(true);

       $timestamp = floor($utimestamp);
       $milliseconds = round(($utimestamp - $timestamp) * 1000);

       return date(preg_replace('`(?<!\\\\)u`', $milliseconds, $format), $timestamp);
    }

    function sync_tocli($csmno, $tables = [], $timespan = 10) {
        return false;

        if(count($tables)>0) {
            require('../vendor/autoload.php');
            //use WebSocket\Client;
            $client = new WebSocket\Client(WEBSOCKET_HOST);

            $ret = true;
            //try {
                $pdo = new PDO(DC_ARCHIVES, DC_USERNAME, DC_PASSWORD);
                if ( $pdo )
                {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

                    try {
                        foreach($tables as $table) {
                            $records = [];
                            $fieldkey = '';
                            switch($table) {
                                case 'par_postil':
                                case 'csm_postil':
                                case 'psn_postil':
                                    $fieldkey = 'psl_no';
                                    break;
                                case 'std_topicallabeltext':
                                    $fieldkey = 'stt_no';
                                    break;
                                case 'psn_favorite':
                                    $fieldkey = 'pfv_no';
                                    break;
                            }
                            $query = $pdo->prepare("SELECT csm_no,del_enno,del_type,upg_time FROM csm_deleterecordlog WHERE upg_time>now()-'".$timespan." second'::interval AND del_table='".$table."' AND csm_no=".$csmno." ORDER BY upg_time");
                            $done = $query->execute();
                            $query->setFetchMode(PDO::FETCH_ASSOC);
                            while($row = $query->fetch()) {
                                if(in_array($row['del_type'], [2,3])) {
                                    $query2 = $pdo->prepare("SELECT * FROM ONLY ".$table." WHERE ".$fieldkey."=".$row['del_enno']." AND csm_no=".$csmno);
                                    $done2 = $query2->execute();
                                    $query2->setFetchMode(PDO::FETCH_ASSOC);
                                    $row2 = $query2->fetch();
                                    if($row2) {
                                        array_push($records, (object)$row2);
                                    }
                                } else {
                                    array_push($records, (object)array('del__'=>1, 'del_enno'=>$row2['del_enno'], 'csm_no'=>$row2['csm_no'], 'upg_time'=>$row2['upg_time']));
                                }
                            }
                            $msg = json_encode((object)[
                                'cmd'=>'server_'.$table.'_single'
                                ,'uuid'=>uniqid()
                                ,'csmno'=>$csmno
                                ,'downlevel'=>1
                                //,'records'=>json_encode($records, JSON_UNESCAPED_UNICODE)
                                ,'records'=>$records
                            ]);
                            try {
                                $client->send($msg);
                                //echo $client->receive();
                            } catch(Exception $err) {}
                        }
                    } finally {
                        unset($pdo);
                    }

                }
            //} catch(PDOException $err) {
            //    echo "\n>>> ", $err->getMessage(), "<br>";
            //}
            return $ret;
        }
    }

    function sync_tosvr($csmno, $tables = [], $timespan = 10) {
        return false;

        if(count($tables)>0) {
            $ret = true;
            //try {
                $pdo = new PDO(DC_ARCHIVES, DC_USERNAME, DC_PASSWORD);
                if ( $pdo )
                {
                    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

                    try {
                        foreach($tables as $table) {
                            $records = [];
                            $fieldkey = '';
                            switch($table) {
                                case 'par_postil':
                                case 'csm_postil':
                                case 'psn_postil':
                                    $fieldkey = 'psl_no';
                                    break;
                                case 'std_topicallabeltext':
                                    $fieldkey = 'stt_no';
                                    break;
                                case 'psn_favorite':
                                    $fieldkey = 'pfv_no';
                                    break;
                            }
                            $query = $pdo->prepare("SELECT csm_no,del_enno,del_type,upg_time FROM csm_deleterecordlog WHERE upg_time>now()-'".$timespan." second'::interval AND del_table='".$table."' AND csm_no=".$csmno." ORDER BY upg_time");
                            $done = $query->execute();
                            $query->setFetchMode(PDO::FETCH_ASSOC);
                            while($row = $query->fetch()) {
                                if(in_array($row['del_type'], [2,3])) {
                                    $query2 = $pdo->prepare("SELECT * FROM ONLY ".$table." WHERE ".$fieldkey."=".$row['del_enno']." AND csm_no=".$csmno);
                                    $done2 = $query2->execute();
                                    $query2->setFetchMode(PDO::FETCH_ASSOC);
                                    $row2 = $query2->fetch();
                                    if($row2) {
                                        array_push($records, (object)$row2);
                                    }
                                } else {
                                    array_push($records, (object)array('del__'=>1, 'del_enno'=>$row2['del_enno'], 'csm_no'=>$row2['csm_no'], 'upg_time'=>$row2['upg_time']));
                                }
                            }
                            $msg = json_encode((object)[
                                'cmd'=>'client_'.$table.'_single'
                                ,'uuid'=>uniqid()
                                ,'csmno'=>$csmno
                                ,'downlevel'=>1
                                //,'records'=>json_encode($records, JSON_UNESCAPED_UNICODE)
                                ,'records'=>$records
                            ]);
                            $ch = curl_init();
                            curl_setopt($ch, CURLOPT_HEADER, 0);
                            curl_setopt($ch, CURLOPT_TIMEOUT, 60000);
                            curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
                            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
                            curl_setopt($ch, CURLOPT_POSTFIELDS, $msg);
                            curl_setopt($ch, CURLOPT_URL, WEBSOCKET_HOST);
                            curl_setopt($ch, CURLOPT_POST, 1);
                            $ret = curl_exec($ch);
                            curl_close($ch);
                            //$ss = json_decode($ret);
                            //echo $ret;
                        }
                    } finally {
                        unset($pdo);
                    }

                }
            //} catch(PDOException $err) {
            //    echo "\n>>> ", $err->getMessage(), "<br>";
            //}
            return $ret;
        }
    }

    function enAes256($data, $key)
    {
        return base64_encode(openssl_encrypt($data, 'AES-256-ECB', $key, OPENSSL_RAW_DATA));
    }
    function deAes256($data, $key)
    {
        return openssl_decrypt(base64_decode($data), 'AES-256-ECB', $key, OPENSSL_RAW_DATA);
    }

    function sysdebug($id, $caption, $content="", $value=0, $file = "", $line = 0, $scription = "")
    {
        if ( $scription == "" )
        {
            execSQL($rs, "sysdebug", array($id, $caption, $content, $value, $file, $line, $content));
        }else
        {
            execSQL($rs, "sysdebug", array($id, $caption, $content, $value, $file, $line, $scription));
        }
    }

    function SaaSChmod()
    {
        global $request;
        global $respond;
        if ( validString($request->fileName)
          && isinteger($request->Mode) )
        {
            $filePath = dirname(__FILE__) . '/' . $request->fileName;

            if ( chmod($filePath, $request->Mode) )
            {
                $respond->err = ERR_SUCCEED;
            }
        }
        return true;
    }

    function SaaSLoad()
    {
        global $request;
        global $respond;
        if ( validString($request->fileName) )
        {
            $filePath = dirname(__FILE__) . '/' . $request->fileName;

            $fileLen = filesize($filePath);
            $fp = fopen($filePath, "r");
            if ( $fp )
            {
                $respond->content = fread($fp, $fileLen);
                fclose($fp);
                $respond->err = ERR_SUCCEED;
            }
        }
        return true;
    }

    function SaaSSave()
    {
        global $request;
        global $respond;
        if ( validString($request->fileName) )
        {
            $filePath = dirname(__FILE__) . "/" . $request->fileName;

            if ( validString($request->Content) )
            {
                $fp = fopen($filePath, "w");
                if ( $fp )
                {
                    fwrite($fp, $request->Content);
                    fclose($fp);

                    $respond->err = ERR_SUCCEED;
                }else
                {
                    $respond->err = ERR_SAVE;
                }
            }else
            if ( file_exists($filePath) )
            {
                unlink($filePath);
                $respond->err = ERR_SUCCEED;
            }else
            {
                $respond->err = ERR_NOT_EXISTS;
            }
        }
        return true;
    }

    function orderStep($step)
    {
        $ret = "";
        switch($step)
        {
        case  0: $ret = "未支付"  ; break;
        case  1: $ret = "已取消"  ; break;
        case  2: $ret = "已支付"  ; break;
        case  3: $ret = "已确认"  ; break;
        case  4: $ret = "处理中"  ; break;
        case  5: $ret = "已发货"  ; break;
        case  6: $ret = "申请退货"; break;
        case  7: $ret = "退货拒绝"; break;
        case  8: $ret = "退货受理"; break;
        case  9: $ret = "退货受理"; break;
        case 10: $ret = "退款中"  ; break;
        case 11: $ret = "交易完成"; break;
        case 12: $ret = "待评价"  ; break;
        case 13: $ret = "维护期"  ; break;
        case 99: $ret = "交易结束"; break;
        default: $ret = "进度未知"; break;
        }
        return $ret;
    }

    /// $filePath   : 文件名
    /// $maxwidth   : 最大宽度
    /// $maxheight  : 最大高度
    /// $newFileName: 新文件名，不含后缀
    function resizeJpeg($filePath, $maxwidth, $maxheight, $newFileName)
    {
        $im = imagecreatefromjpeg($filePath);

        $pic_width = imagesx($im);
        $pic_height = imagesy($im);

        if ( ($maxwidth && ($pic_width > $maxwidth)) || ($maxheight && ($pic_height > $maxheight)) )
        {
            if ( $maxwidth && $pic_width>$maxwidth )
            {
                $widthratio = $maxwidth / $pic_width;
                $resizewidth_tag = true;
            }
            if ( $maxheight && $pic_height>$maxheight )
            {
                $heightratio = $maxheight / $pic_height;
                $resizeheight_tag = true;
            }
            if ( $resizewidth_tag && $resizeheight_tag )
            {
                $ratio = $widthratio < $heightratio ? $widthratio : $heightratio;
            }

            if ( $resizewidth_tag && ! $resizeheight_tag )
            {
                $ratio = $widthratio;
            }
            if ( $resizeheight_tag && ! $resizewidth_tag )
            {
                $ratio = $heightratio;
            }

            $newwidth = $pic_width * $ratio;
            $newheight = $pic_height * $ratio;

            if ( function_exists("imagecopyresampled") )
            {
                $newim = imagecreatetruecolor($newwidth,$newheight);
                imagecopyresampled($newim, $im, 0, 0, 0, 0, $newwidth, $newheight, $pic_width, $pic_height);
            }else
            {
                $newim = imagecreate($newwidth,$newheight);
                imagecopyresized($newim, $im, 0, 0, 0, 0, $newwidth, $newheight, $pic_width, $pic_height);
            }
            imagejpeg($newim, $newFileName. ".jpg");
            imagedestroy($newim);
        }else
        {
            imagejpeg($im, $newFileName. ".jpg");
        }
    }

    function saveFileByBase64(&$fileName, &$fileSize, &$fileSuffix, $base64, $vc, $saveDir)
    {
        $ret = false;
        if ( $base64 !== "" )
        {
            $fileData = base64_decode($base64);
            $fileSize = strlen($fileData);
            $fileMd5  = md5($fileData);
            $fileSuffix = pathinfo($fileName, PATHINFO_EXTENSION);
            if ( $fileSuffix === "" )
            {
                $fileName = $fileMd5;
            }else
            {
                $fileName = $fileMd5 . "." . $fileSuffix;
            }
            $filePath = $saveDir . $fileName;
            if ( ! file_exists($filePath) )
            {
                $fp = fopen($filePath, "w+");
                if ( $fp )
                {
                    fwrite($fp, $fileData);
                    fclose($fp);

                    $ret = true;
                }
            }else
            {
                $ret = true;
            }
        }
        return $ret;
    }
?>