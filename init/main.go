package main

import (
	"bufio"
	"database/sql"
	"fmt"
	_ "github.com/go-sql-driver/mysql"
	"io"
	"os"
	"strings"
)

//输出示例
func sample(){
	fmt.Println("示例: ./initdb test.sql 127.0.0.1:3306 root:123456")
	fmt.Println("test.sql  初始化数据库的sql文件")
	fmt.Println("127.0.0.1:3306  mysql数据库的IP和PORT ")
	fmt.Println("root:123456  mysql数据库的root账号和密码 ")
}


//读取sql脚本
func readSql(fileName string)  (map[int]string ,int) {
	sql_map := make(map[int]string)
	var sql string = ""
	var key int = 0

	file,err := os.Open(fileName)
	if err != nil {
		fmt.Println("open ",fileName," failed!")
		fmt.Println(err)
		os.Exit(-1)
	}
	defer file.Close()
	buf := bufio.NewReader(file)

	for {
		line,err := buf.ReadString('\n')
		line = strings.TrimSpace(line)
		if err != nil || io.EOF == err {
			sql += line
			sql_map[key] = sql
			key += 1
			break
		}

		if line == "" {
			sql_map[key] = sql
			key += 1
			sql = ""
		}else{
			sql += line
		}
	}

	return sql_map, key
}


//执行数据库初始化
func execSQL(mysql map[int]string,sqlNum int,ipPort string,user string)  {

	db,err := sql.Open("mysql",user+"@tcp(" + ipPort + ")/mysql?charset=utf8")
	if err != nil {
		fmt.Println("ERROR:", err)
		os.Exit(-1)
	}
	defer db.Close()
	fmt.Println("连接数据库成功!")

	for i:= 0;i < sqlNum; i++ {
		_,err := db.Exec(mysql[i])
		if err != nil {
			fmt.Println("执行第",i+1,"条语句失败！")
			fmt.Println("ERROR:",err)
			os.Exit(-1)
		}
		fmt.Println("执行第",i+1,"条语句成功！")
	}
}



func main() {

	sample()

	args := os.Args
	if args == nil || len(args) < 4 {
		fmt.Println("err:输入的参数错误!")
		return
	}

	fileName := args[1]
	ipPort := args[2]
	user := args[3]

	fmt.Println("fileName=",fileName,"  ip:port=",ipPort,"  user:pw=",user)

	mysql := make(map[int]string)
	var sqlNum int

	mysql,sqlNum = readSql(fileName)
	execSQL(mysql,sqlNum,ipPort,user)
}
