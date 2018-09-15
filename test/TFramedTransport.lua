--
-- Licensed to the Apache Software Foundation (ASF) under one
-- or more contributor license agreements. See the NOTICE file
-- distributed with this work for additional information
-- regarding copyright ownership. The ASF licenses this file
-- to you under the Apache License, Version 2.0 (the
-- "License"); you may not use this file except in compliance
-- with the License. You may obtain a copy of the License at
--
--   http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing,
-- software distributed under the License is distributed on an
-- "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
-- KIND, either express or implied. See the License for the
-- specific language governing permissions and limitations
-- under the License.
--

require 'TTransport'
require 'libluabpack'


--[[
print_dump是一个用于调试输出数据的函数，能够打印出nil,boolean,number,string,table类型的数据，以及table类型值的元表
参数data表示要输出的数据
参数showMetatable表示是否要输出元表
参数lastCount用于格式控制，用户请勿使用该变量
]]

function print_dump(data, showMetatable, lastCount)
    if type(data) ~= "table" then
        --Value
        if type(data) == "string" then
            io.write("\"", data, "\"")
        else
            io.write(tostring(data))
        end
    else
        --Format
        local count = lastCount or 0
        count = count + 1
        io.write("{\n")
        --Metatable
        if showMetatable then
            for i = 1,count do 
                io.write("\t") 
            end
            local mt = getmetatable(data)
            io.write("\"__metatable\" = ")
            print_dump(mt, showMetatable, count)    -- 如果不想看到元表的元表，可将showMetatable处填nil
            io.write(",\n")        --如果不想在元表后加逗号，可以删除这里的逗号
        end
        --Key
        for key,value in pairs(data) do
            for i = 1,count do 
                io.write("\t") 
            end
            if type(key) == "string" then
                io.write("\"", key, "\" = ")
            elseif type(key) == "number" then
                io.write("[", key, "] = ")
            else
                io.write(tostring(key))
            end
            print_dump(value, showMetatable, count)    -- 如果不想看到子table的元表，可将showMetatable处填nil
            io.write(",\n")        --如果不想在table的每一个item后加逗号，可以删除这里的逗号
        end
        --Format
        for i = 1,lastCount or 0 do 
            io.write("\t") 
        end
            io.write("}")
    end
    --Format
    if not lastCount then
        io.write("\n")
    end
end



TFramedTransport = TTransportBase:new{
  __type = 'TFramedTransport',
  doRead = true,
  doWrite = true,
  wBuf = '',
  rBuf = ''
}

function TFramedTransport:new(obj)
  if ttype(obj) ~= 'table' then
    error(ttype(self) .. 'must be initialized with a table')
  end

  -- Ensure a transport is provided
  if not obj.trans then
    error('You must provide ' .. ttype(self) .. ' with a trans')
  end

  return TTransportBase.new(self, obj)
end

function TFramedTransport:isOpen()
  return self.trans:isOpen()
end

function TFramedTransport:open()
  return self.trans:open()
end

function TFramedTransport:close()
  return self.trans:close()
end

function TFramedTransport:read(len)
  if string.len(self.rBuf) == 0 then
    self:__readFrame()
  end

  if self.doRead == false then
    return self.trans:read(len)
  end

  if len > string.len(self.rBuf) then
    local val = self.rBuf
    self.rBuf = ''
    return val
  end

  local val = string.sub(self.rBuf, 0, len)
  self.rBuf = string.sub(self.rBuf, len+1)
  return val
end

function TFramedTransport:__readFrame()
  local buf = self.trans:readAll(4)
  local frame_len = libluabpack.bunpack('i', buf)
  self.rBuf = self.trans:readAll(frame_len)
end


function TFramedTransport:write(buf, len)
  if self.doWrite == false then
    return self.trans:write(buf, len)
  end

  if len and len < string.len(buf) then
    buf = string.sub(buf, 0, len)
  end
  self.wBuf = self.wBuf .. buf
end

--[[
function TFramedTransport:flush()
  if self.doWrite == false then
    return self.trans:flush()
  end

  -- If the write fails we still want wBuf to be clear
  local tmp = self.wBuf
  --self.wBuf = ''
  local frame_len_buf = libluabpack.bpack("i", string.len(tmp))
  self.wBuf = frame_len_buf .. self.wBuf
  self.trans:write(self.wBuf)
  --self.trans:write(frame_len_buf)
  --self.trans:write(tmp)
  self.wBuf = ''
  self.trans:flush()
end
--]]

function TFramedTransport:flush()
  if self.doWrite == false then
    return self.trans:flush()
  end

  -- If the write fails we still want wBuf to be clear
  local tmp = self.wBuf
  self.wBuf = ''
  local frame_len_buf = libluabpack.bpack("i", string.len(tmp))
  tmp = frame_len_buf .. tmp
  self.trans:write(tmp)
  self.trans:flush()
end

TFramedTransportFactory = TTransportFactoryBase:new{
  __type = 'TFramedTransportFactory'
}
function TFramedTransportFactory:getTransport(trans)
  if not trans then
    terror(TProtocolException:new{
      message = 'Must supply a transport to ' .. ttype(self)
    })
  end
  return TFramedTransport:new{trans = trans}
end
