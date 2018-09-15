
package.cpath = package.cpath .. ";/usr/local/lib/?.so"

require('interface_BRKService')
require('TSocket')
require('TBufferedTransport')
require('TFramedTransport')
require('THttpTransport')
require('TCompactProtocol')
require('TJsonProtocol')
require('TBinaryProtocol')
require('liblualongnumber')

-- import luaunit
local lu = require('luaunit')

local client

local opt = {
    host      = '47.106.79.26',
    port      = '9090',
    protocol  = TCompactProtocol,
    transport = TFramedTransport
}

TestProtocol = {}

function TestProtocol:setUp()
    local socket = TSocket:new{
        host = opt.host,
        port = tonumber(opt.port),
        timeout = 5000
    }
    assert(socket, 'Failed to create client socket')

    local transport = opt.transport:new{
        trans = socket,
        isServer = false
    }

    local protocol = opt.protocol:new{
        trans = transport
    }
    assert(protocol, 'Failed to create binary protocol')

    client = BRKServiceClient:new{
        protocol = protocol
    }
    assert(client, 'Failed to create client')

    -- Open the transport
    local status, _ = pcall(transport.open, transport)
    assert(status, 'Failed to connect to serverle_code')
end

--[[
function TestProtocol:test_brk_get_mobile_code_right()
    --print( "called this unit case")
    local r = client:brk_get_mobile_code('15200599665')
    lu.assertEquals(r.resultCode, 200)
end


function TestProtocol:test_brk_user_login_success()
     --print( "called this unit case")
     local r = client:brk_login('15200599665', '084135')
     lu.assertEquals(r.resultCode, 200)
end


function TestProtocol:test_brk_recharge()
     --print( "called this unit case")
     local r = client:brk_recharge('15200599665', 100)
     lu.assertEquals(r.resultCode, 200)
end


function TestProtocol:test_brk_multi_recharge()
     --print( "called this unit case")
     local r = client:brk_get_account_balance('15200599665')
     local balance = r.balance + 100
     
     r = client:brk_recharge('15200599665', 100)
     lu.assertEquals(r.resultCode, 200)
     
     r = client:brk_get_account_balance('15200599665')
     
     lu.assertEquals(r.balance, balance)
end

function TestProtocol:test_brk_get_account_balance()
     --print( "called this unit case")
     local r = client:brk_get_account_balance('15200599665')
     lu.assertEquals(r.resultCode, 200)
     lu.assertIsTrue(r.balance > 0)
     local br = "user 15200599665 balance is " .. r.balance
     print(br)
end


function TestProtocol:test_brk_unlock_success()
     --print( "called this unit case")
     local r = client:brk_unlock('15200599665', '10275846')
     lu.assertEquals(r.resultCode, 200)
end

function TestProtocol:test_brk_lock_success()
     --print( "called this unit case")
     local r = client:brk_lock('15200599665', '10275846')
     lu.assertEquals(r.resultCode, 200)
end
]]--

function TestProtocol:test_brk_list_travel_success()
     --print( "called this unit case")
     local r = client:brk_list_travel('15200599665')
     lu.assertEquals(r.resultCode, 200)
     lu.assertIsTrue(r.mileage > 0)
     lu.assertIsTrue(r.discharge > 0)
     lu.assertIsTrue(r.calorie > 0)
     local mileage = "mileage is " .. r.mileage
     print(mileage)
     local discharge = "discharge is " .. r.discharge
     print(discharge)
     local calorie = "r.calorie is " .. r.calorie 
     print(calorie)
end

function TestProtocol:tearDown()
    if client then
        -- close the connection
        client:close()
    end
end

local runner = lu.LuaUnit.new()
runner:setOutputType("tap")
os.exit( runner:runSuite() )

--os.exit( lu.LuaUnit.run() )
