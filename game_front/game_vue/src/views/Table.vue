<template>
<div>
    <el-container>
        <el-header>
            <el-page-header @back="leave_table" :content="page_title_no()" title="离开">
            </el-page-header>
        </el-header>
        <el-main>
            <el-row :gutter="10">
                <el-col :span="8" v-for="player in get_first_line()" :key="player.seat_no">
                    <EachPlayer :dealer_pos="dealer_pos" :action_pos="action_pos" :seat_no="player.seat_no" :player_info="get_player_info(player.seat_no)" :sit_down_proc_in="player_sit_down_proc" :show_sit_down="!is_player_sit"></EachPlayer>
                </el-col>
            </el-row>
            <el-row :gutter="10">
                <el-col :span="8" v-for="player in get_second_line()" :key="player.seat_no">
                    <EachPlayer :dealer_pos="dealer_pos" :action_pos="action_pos" :seat_no="player.seat_no" :player_info="get_player_info(player.seat_no)" :sit_down_proc_in="player_sit_down_proc" :show_sit_down="!is_player_sit"></EachPlayer>
                </el-col>
            </el-row>
            <el-row :gutter="20">
                <el-col :span="10">
                    <el-row v-if="is_player_sit">
                        <el-col :span="12">
                            <el-image :src="first_hand_card" fit="fill">
                                <div slot="error">没牌</div>
                            </el-image>
                        </el-col>
                        <el-col :span="12">
                            <el-image :src="second_hand_card" fit="fill">
                                <div slot="error">没牌</div>
                            </el-image>
                        </el-col>
                    </el-row>
                </el-col>
                <el-col :span="14">
                    <div v-if="action_pos == self_seat && self_seat > 0">
                        <el-slider v-model="bat_cash_value" :min="min_bat_cash*2" :max="self_total_cash()" :step="5">
                        </el-slider>
                        <el-row>
                            <el-col :span="24">
                                <el-button v-if="self_bat_cash() < min_bat_cash" type="primary" @click="follow_prev_click">跟注</el-button>
                                <el-button v-else type="primary" @click="follow_prev_click">过牌</el-button>
                            </el-col>
                        </el-row>
                        <el-row>
                            <el-col :span="24">
                                <el-button v-if="self_bat_cash() == 0" type="primary" @click="raise_action_click">下注</el-button>
                                <el-button v-else type="primary" @click="raise_action_click">加注</el-button>
                            </el-col>
                        </el-row>
                        <el-row>
                            <el-col :span="24">
                                <el-button type="danger" @click="fall_action_click">弃牌</el-button>
                            </el-col>
                        </el-row>
                    </div>
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
            dealer_pos: -1,
            action_pos: -1,
            bat_cash_value:0,
            min_bat_cash:0,
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
            },
            first_hand_card: "",
            second_hand_card: "",
            self_total_cash: function() {
                return this.get_player_info(this.self_seat).total_cash + this.get_player_info(this.self_seat).bat_cash;
            },
            self_bat_cash:function () {
                return this.get_player_info(this.self_seat).bat_cash;
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
        },
        make_hand_card_url: function (_num, _color) {
            var color_name = "";
            switch (_color) {
                case 0:
                    color_name = "spade";
                    break;
                case 1:
                    color_name = "heart";
                    break;
                case 2:
                    color_name = "club";
                    break;
                case 3:
                    color_name = "diamond";
                    break;

                default:
                    break;
            }
            var url = "http://www.d8sis.cn/game_resource/cards_imgs/" + color_name + "_" + _num + ".jpg";
            return url;
        },
        follow_prev_click:function () {
            var action_msg = new messages.player_action();
            action_msg.setAction(0);
            action_msg.setCash(this.min_bat_cash);
            this.send_via_websocket(18, action_msg);
        },
        raise_action_click:function() {
            var action_msg = new messages.player_action();
            action_msg.setAction(0);
            action_msg.setCash(this.bat_cash_value);
            this.send_via_websocket(18, action_msg);
        },
        fall_action_click:function() {
            var action_msg = new messages.player_action();
            action_msg.setAction(1);
            action_msg.setCash(0);
            this.send_via_websocket(18, action_msg);
        },
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
                var dgram_content = from_server_data.slice(8, cur_length + 8);
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
                                bat_cash: 0,
                                is_fall: false,
                                is_allin: false,
                            });
                        }
                        var table_info_msg = messages.table_info_sync.deserializeBinary(dgram_content);
                        vue_this.table_no = table_info_msg.getTableNo();
                        vue_this.dealer_pos = table_info_msg.getDealerPos();
                        vue_this.action_pos = table_info_msg.getActionPos();
                        vue_this.min_bat_cash = table_info_msg.getMinBat();
                        var all_players = table_info_msg.getPlayersList();
                        all_players.forEach(element => {
                            var cur_seat_no = element.getSeatNo();
                            var player_got = {
                                seat_no: element.getSeatNo(),
                                name: element.getName(),
                                logo: element.getLogo(),
                                total_cash: element.getTotalCash(),
                                bat_cash: element.getBatCash(),
                                is_fall: element.getIsFall(),
                                is_allin: element.getIsAllIn(),
                            };
                            vue_this.$set(vue_this.sit_down_player, cur_seat_no, player_got)
                        });
                        console.log(vue_this);
                        break;
                    case 14:
                        var self_info_msg = messages.player_self_info.deserializeBinary(dgram_content);
                        vue_this.self_seat = self_info_msg.getSeatNo();
                        if (vue_this.self_seat == -1)
                            vue_this.is_player_sit = false;
                        else
                            vue_this.is_player_sit = true;
                        break;
                    case 17:
                        var hand_card_info = messages.player_hand_card_info.deserializeBinary(dgram_content);
                        vue_this.first_hand_card = vue_this.make_hand_card_url(hand_card_info.getFirst().getNumber(), hand_card_info.getFirst().getColor());
                        vue_this.second_hand_card = vue_this.make_hand_card_url(hand_card_info.getSecond().getNumber(), hand_card_info.getSecond().getColor());
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
