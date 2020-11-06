<template>
    <div >
        <div v-if="took_by_other">
            网络断开，请刷新重试
        </div>
        <div v-else>
            {{table_no}}
        </div>
    </div>

</template>

<script>
import messages from '../game_msg_pb'
export default {
    name: 'Table',
    data: function() {
        return {
            table_no: 0,
            took_by_other: false
        }
    },
    methods: {
        send_via_websocket: function(_type, _data) {
            var tlv_v = _data.serializeBinary();
            var tlv_t = _type;
            var tlv_l = tlv_v.byteLength;
            var buffer = new ArrayBuffer(tlv_l + 8);
            new DataView(buffer, 0).setUint32(0, tlv_t);
            new DataView(buffer, 4).setUint32(0, tlv_l);
            new Uint8Array(buffer, 8).set(tlv_v);
            this.game_ws.send(buffer);
            console.log(buffer);
        },
    },
    beforeMount: function() {
        var vue_this = this;
        this.table_no = this.$route.params.table_no;
        this.game_ws = new WebSocket('ws://59.110.64.232/game', 'binary');
        this.game_ws.binaryType = "arraybuffer";
        this.game_ws.onopen = function(evt) {
            console.log("web socket openned" + evt);
        }
        this.game_ws.onclose = function(evt) {
            console.log("web socket closed" + evt);
            vue_this.took_by_other = true;
        }
        this.game_ws.onerror = function(evt) {
            console.log('web socket error' + evt);
        }
        this.game_ws.onmessage = function(evt) {
            var total_length = evt.data.byteLength;
            var left_length = total_length;
            while (left_length > 0) {
                var from_server_data = evt.data.slice(total_length - left_length, total_length);
                var cur_length = new DataView(from_server_data, 4).getUint32(0);
                left_length -= cur_length + 8;
                var cur_type = new DataView(from_server_data, 0).getUint32();
                switch (cur_type) {
                    case 0:
                        var send_data = new messages.sync_session();
                        send_data.setSession(vue_this.$cookies.get('ssid'));
                        send_data.setTableNo(vue_this.table_no);
                        vue_this.send_via_websocket(0, send_data);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
</script>

<style>

</style>