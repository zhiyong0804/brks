
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

local client

local opt = {
    host      = '47.106.79.26',
    port      = '9090',
    protocol  = TCompactProtocol,
    transport = TFramedTransport
}

function teardown()
    if client then
        -- close the connection
        client:close()
    end
end

function assertEqual(val1, val2, msg)
  assert(val1 == val2, msg)
end

function testBasicClient()
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

    local r = client:brk_get_mobile_code('15200599665')
    print(r.resultCode)
    print(r.resultMsg)
    assertEqual(r.resultCode, 200, 'Failed call brk_get_mobile_code')
    --sleep(2)
end

testBasicClient()
teardown()
