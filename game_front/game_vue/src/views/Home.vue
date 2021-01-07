<template>
<div class="home">
    <h1 class='outlinedB' style="height: 10%">扑克</h1>
    <div class="main-window" v-if="!is_login">
        <el-row :gutter="20">
            <el-button type="success" round @click="nav_to_wechat_login">
                微信登陆
            </el-button>
        </el-row>
        <div>
            <el-button @click="random_login">游客登录</el-button>
        </div>
        <el-link type="info" href="http://beian.miit.gov.cn" id="beian">京ICP备2020039126号-1</el-link>
    </div>
    <div class="main-window" v-else>
        <el-container style="height: 100%">
            <el-main style="height: 90%">
                <el-card class="user_info" :body-style="{ 'background-color': 'rgb(225, 243, 216)'}">
                    <el-row :gutter="20">
                        <el-col :span="12">
                            <el-avatar :src="user_logo" :size="100"></el-avatar>
                        </el-col>
                        <el-col :span="12">
                            <h3 class="dilate">{{user_name}}</h3>
                            <i class="el-icon-money distant-top">
                                {{user_cash}}
                            </i>
                        </el-col>
                    </el-row>
                </el-card>
                <el-divider></el-divider>
                <el-row :gutter="20">
                    <el-col :span="12">
                        <el-input-number v-model="added_cash" :step="1000" :max="50000"></el-input-number>
                    </el-col>
                    <el-col :span="12">
                        <el-button type="primary" @click="add_cash">充值</el-button>
                    </el-col>
                </el-row>
                <el-row :gutter="20">
                    <el-col :span="8">
                        <el-button type="primary" @click="create_table">创建牌桌</el-button>
                    </el-col>
                    <el-col :span="8">
                        <el-button type="primary" @click="enter_random">随机入桌</el-button>
                    </el-col>
                    <el-col :span="8">
                        <el-button type="primary" @click="table_no_dialog = true">按号入桌</el-button>
                    </el-col>
                </el-row>
                <el-button type="success" v-if="belong_to_table != -1" @click="re_con_table">重连到{{belong_to_table}}号桌</el-button>
            </el-main>
            <el-footer style="height: 10%">
                <el-button type="danger" @click="logoff">退出登录</el-button>
            </el-footer>
        </el-container>
        <el-dialog :visible.sync="table_no_dialog" :modal-append-to-body="false" :show-close="false">
            <el-input v-model="table_no_enter" placeholder="请输入桌号"></el-input>
            <el-row :gutter="20" style="margin-top: 10px">
                <el-col :span="12">
                    <el-button type="primary" @click="enter_table">确 定</el-button>
                </el-col>
                <el-col :span="12">
                    <el-button @click="table_no_dialog= false">取 消</el-button>
                </el-col>
            </el-row>
        </el-dialog>
    </div>
</div>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from '@/components/HelloWorld.vue'
import {
    Base64
} from 'js-base64'

export default {
    name: 'Home',
    data: function () {
        return {
            user_name: '',
            user_logo: '',
            user_cash: '',
            is_login: false,
            added_cash: 1000,
            belong_to_table: -1,
            table_no_dialog: false,
            table_no_enter: "",
        };
    },
    methods: {
        nav_to_wechat_login: function () {
            window.location.href = "https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa390f8b6f68e9c6d&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_userinfo&state=STATE#wechat_redirect";
        },
        get_cur_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get('http://www.d8sis.cn/game_rest/userinfo/' + ssid).then(function (resp) {
                if (resp.data.result.user_name.length > 0) {
                    Base64.extendString();
                    vue_this.user_name = resp.data.result.user_name.fromBase64();
                    vue_this.user_logo = 'http://www.d8sis.cn' + resp.data.result.user_logo;
                    vue_this.user_cash = resp.data.result.user_cash;
                    vue_this.is_login = true;
                    vue_this.belong_to_table = resp.data.result.table_no;
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
            });
        },
        logoff: function () {
            var ssid = this.$cookies.get('ssid');
            this.$axios.get('http://www.d8sis.cn/game_rest/logoff/' + ssid).then(function (resp) {
                console.log(resp);
                window.location.replace('/');
            }).catch(function (err) {
                console.log(err);
            });
        },
        add_cash: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.post('http://www.d8sis.cn/game_rest/add_cash', {
                text: {
                    ssid: ssid,
                    cash: this.added_cash
                }
            }).then(function (resp) {
                if (resp.data.result == 'success') {
                    vue_this.get_cur_user_info();
                    vue_this.$notify({
                        title: '加钱成功',
                        message: '',
                        type: 'success'
                    });
                } else {
                    vue_this.$notify({
                        title: '加钱失败',
                        message: '',
                        type: 'error'
                    });
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
        create_table: function () {
            var vue_this = this;
            this.$axios.get('http://www.d8sis.cn/game_rest/create_table').then(function (resp) {
                if ("success" == resp.data.result.created) {
                    console.log("table no is" + resp.data.result.table_no);
                    vue_this.$router.push({
                        path: '/table/' + resp.data.result.table_no
                    });
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
        enter_random: function () {},
        enter_table: function () {
            this.table_no_dialog = false;

            this.$router.push({
                path: '/table/' + this.table_no_enter
            });
        },
        re_con_table: function () {
            this.$router.push({
                path: '/table/' + this.belong_to_table
            });
        },
        random_login: function () {
            var vue_this = this;
            this.$axios.get('http://www.d8sis.cn/game_rest/random_login').then(function (resp) {
                vue_this.$cookies.set("ssid", resp.data.result);
                window.location.replace('/');
            }).catch(function (err) {
                console.log(err);
            });
        }
    },
    beforeMount: function () {
        this.get_cur_user_info();
    }
}
</script>

<style lang="scss" scoped>
.clearfix:before,
.clearfix:after {
    display: table;
    content: "";
}

.clearfix:after {
    clear: both
}

.el-row {
    margin-bottom: 20px;

    &:last-child {
        margin-bottom: 0;
    }

}

.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
}

.el-input-number {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
}

.el-main {
    padding-bottom: 110px;
}

#beian {
    position: absolute;
    bottom: 0px;
    left: 0px;
}

.user_info {
    margin-bottom: 20px;
}

.home {
    position: fixed;
    background-color: rgb(253, 246, 236);
    height: 100%;
    width: 100%;
    text-align: center;
}

.main-window {
    height: 90%;
}

@import "../assets/self-defined-style.css";
</style>
