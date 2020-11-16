<template>
<div>
    <el-container>
        <el-header>
            <el-page-header @back="leave_table" :content="page_title_no()" title="离开">
            </el-page-header>
        </el-header>
        <el-main>
            <el-row :gutter="20">
                <el-col :span="8" v-for="player in get_first_line()" :key="player.seat_no">
                    <EachPlayer :seat_no="player.seat_no" :player_info="get_player_info(player.seat_no)" :sit_down_proc_in="player_sit_down_proc" :show_sit_down="!is_player_sit"></EachPlayer>
                </el-col>
            </el-row>
            <el-row :gutter="20">
                <el-col :span="8" v-for="player in get_second_line()" :key="player.seat_no">
                    <EachPlayer :seat_no="player.seat_no" :player_info="get_player_info(player.seat_no)" :sit_down_proc_in="player_sit_down_proc" :show_sit_down="!is_player_sit"></EachPlayer>
                </el-col>
            </el-row>
        </el-main>
        <el-footer>
            <el-button type="danger" @click="player_standup_proc" v-if="is_player_sit">起立</el-button>
        </el-footer>
    </el-container>
</div>
</template>

<script>
import messages from '../game_msg_pb'
import EachPlayer from '@/components/EachPlayer.vue'
export default {
    name: 'Table',
    components: {
        EachPlayer
    },
    data: function () {
        return {
            table_no: 0,
            self_seat: -1,
            sit_down_player: [],
            get_player_info: function (_seat_no) {
                return this.sit_down_player[_seat_no];
            },
            get_first_line: function () {
                return [
                    this.sit_down_player[(this.self_seat + 4) % 6],
                    this.sit_down_player[(this.self_seat + 3) % 6],
                    this.sit_down_player[(this.self_seat + 2) % 6],
                ];
            },
            get_second_line: function () {
                console.log(this.self_seat % 6);
                return [
                    this.sit_down_player[(this.self_seat + 5) % 6],
                    this.sit_down_player[(this.self_seat + 6) % 6],
                    this.sit_down_player[(this.self_seat + 1) % 6],
                ];
            },
            is_player_sit: false,
            page_title_no: function () {
                return "桌号：" + this.table_no;
            }
        }
    },
    methods: {
        send_via_websocket: function (_type, _data) {
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
        send_empty_via_websocket: function (_type) {
            var tlv_t = _type;
            var tlv_l = 0;
            var buffer = new ArrayBuffer(tlv_l + 8);
            new DataView(buffer, 0).setUint32(0, tlv_t);
            new DataView(buffer, 4).setUint32(0, tlv_l);
            this.game_ws.send(buffer);
            console.log(buffer);
        },
        player_sit_down_proc: function (_seat_no, _carry_cash) {
            console.log("some one" + _seat_no + "sit down");
            var sit_req_msg = new messages.player_sit_down_req();
            sit_req_msg.setSeatNo(_seat_no);
            sit_req_msg.setCarryCash(_carry_cash);

            this.send_via_websocket(13, sit_req_msg);
        },
        player_standup_proc: function () {
            this.send_empty_via_websocket(15);
        },
        leave_table: function () {
            window.location.replace('/');
        }
    },
    beforeMount: function () {
        var vue_this = this;
        this.table_no = this.$route.params.table_no;
        this.game_ws = new WebSocket('ws://59.110.64.232/game', 'binary');
        this.game_ws.binaryType = "arraybuffer";
        this.game_ws.onopen = function (evt) {
            console.log("web socket openned" + evt);
        }
        this.game_ws.onclose = function (evt) {
            console.log("web socket closed" + evt);
            vue_this.$router.push({
                path: '/'
            });
        }
        this.game_ws.onerror = function (evt) {
            console.log('web socket error' + evt);
        }
        this.game_ws.onmessage = function (evt) {
            var total_length = evt.data.byteLength;
            var left_length = total_length;
            while (left_length > 0) {
                var from_server_data = evt.data.slice(total_length - left_length, total_length);
                console.log("from server data:");
                console.log(from_server_data);
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
                    case 11:
                        for (let index = 0; index < 6; index++) {
                            vue_this.$set(vue_this.sit_down_player, index, {
                                seat_no: index,
                                name: "无玩家",
                                logo: "",
                                total_cash: 0,
                                bat_cash: 0
                            });
                        }
                        var table_info_msg = messages.table_info_sync.deserializeBinary(from_server_data.slice(8));
                        vue_this.table_no = table_info_msg.getTableNo();
                        var all_players = table_info_msg.getPlayersList();
                        all_players.forEach(element => {
                            var cur_seat_no = element.getSeatNo();
                            var player_got = {
                                seat_no: element.getSeatNo(),
                                name: element.getName(),
                                logo: element.getLogo(),
                                total_cash: element.getTotalCash(),
                                bat_cash: element.getBatCash(),
                            };
                            vue_this.$set(vue_this.sit_down_player, cur_seat_no, player_got)
                        });
                        break;
                    case 14:
                        var self_info_msg = messages.player_self_info.deserializeBinary(from_server_data.slice(8));
                        vue_this.self_seat = self_info_msg.getSeatNo();
                        if (vue_this.self_seat == -1)
                            vue_this.is_player_sit = false;
                        else
                            vue_this.is_player_sit = true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
</script>

<style lang="scss" scoped>
.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
}

.el-main {
    padding-bottom: 110px;
}

.el-footer {
    position: absolute;
    bottom: 0;
    width: 95%;
    height: 100px;
}
</style>
