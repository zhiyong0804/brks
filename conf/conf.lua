--所有的配置信息
db_argv = { 
    db_ip="127.0.0.1", 
    db_port=3306, 
    db_user="root", 
    db_pwd="123456", 
    db_name="dongnaobike"
}

svr_port  = 9090

-- 以下是配置检查
print("检查配置:")
function checkpro(argv, sport)
    --检查端口正确性
    if type(argv.db_port) ~= "number" or argv.db_port > 65535 or argv.db_port < 0 then
        print("端口必须为整数");
        return nil
    end

    -- 检查ip
    head,tail,A,B,C,D=string.find(argv.db_ip, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
    --检查函数
    local ck = function(n)
            if tonumber(n)>255 or tonumber(n)<0 then
                return false
            end
            return true
        end

    if ck(A) and ck(B) and ck(C) and ck(D) then
        --重新格式化字符串避免有空格出错
        argv.db_ip = string.format("%d.%d.%d.%d",A,B,C,D)
    else
        print(string.format("IP范围有错:%s.%s.%s.%s",A,B,C,D))
        return nil
    end

    if sport < 1024 then
        sport = 9090
    end

    print("测试pass")
    return argv, sport
    --return argv
end

return checkpro(db_argv, svr_port)

