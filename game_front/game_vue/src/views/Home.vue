<template>
<div class="home">
    <div v-if="!is_login">
        <h1>这是首页的所要求的的内容</h1>
        <el-row :gutter="20">
            <el-col :span="12">
                <el-button type="success" round @click="nav_to_wechat_login">
                    微信登陆
                </el-button>
            </el-col>
            <el-col :span="12">
                <el-button type="primary" round @click="nav_to_qq_login">
                    <img src='http://qzonestyle.gtimg.cn/qzone/vas/opensns/res/img/Connect_logo_4.png'/>
                </el-button>
            </el-col>
        </el-row>
        <div>
            <el-button @click="random_login">游客登录</el-button>
        </div>
    </div>
    <div v-else>
        <el-container>
            <el-header>
                <el-row :gutter="20">
                    <el-col :span="12">
                        <el-avatar :src="user_logo" :size="50"></el-avatar>
                    </el-col>
                    <el-col :span="12">
                        {{user_name}}
                    </el-col>
                </el-row>
            </el-header>
            <el-main>
                <el-row :gutter="20">
                    <el-col :span="12">
                        <i class="el-icon-money">当前金额</i>
                    </el-col>
                    <el-col :span="12">
                        {{user_cash}}
                    </el-col>
                </el-row>
                <el-row :gutter="20">
                    <el-col :span="6">
                        充值金额
                    </el-col>
                    <el-col :span="12">
                        <el-input-number v-model="added_cash" :step="1000" :max="50000"></el-input-number>
                    </el-col>
                    <el-col :span="6">
                        <el-button type="primary" @click="add_cash">加钱</el-button>
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
                        <el-button type="primary" @click="enter_table">按号入桌</el-button>
                    </el-col>
                </el-row>
                <el-button type="success" v-if="belong_to_table != -1" @click="re_con_table">重连到{{belong_to_table}}号桌</el-button>
            </el-main>
            <el-footer>
                <el-button type="danger" @click="logoff">退出登录</el-button>
            </el-footer>
        </el-container>
    </div>
    <el-link type="info" href="http://beian.miit.gov.cn" id="beian">京ICP备2020039126号-1</el-link>
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
        };
    },
    methods: {
        nav_to_wechat_login: function () {
            window.location.href = "https://open.weixin.qq.com/connect/qrconnect?appid=wx987b51617d4be3ae&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_login&state=STATE#wechat_redirect";
        },
        nav_to_qq_login: function () {
            QC.Login.showPopup({
                appId: "101912803",
                redirectURI: "http://www.d8sis.cn/qqlogin"
            });
        },
        get_cur_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get('http://www.d8sis.cn/game_rest/userinfo/' + ssid).then(function (resp) {
                if (resp.data.result.user_name.length > 0) {
                    Base64.extendString();
                    vue_this.user_name = resp.data.result.user_name.fromBase64();
                    vue_this.user_logo = resp.data.result.user_logo;
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
        enter_random: function () {

        },
        enter_table: function () {

        },
        re_con_table: function () {
            this.$router.push({
                path: '/table/' + this.belong_to_table
            });
        },
        random_login:function() {
            var vue_this = this;
            this.$axios.get('http://www.d8sis.cn/game_rest/random_login').then(function(resp) {                    
                vue_this.$cookies.set("ssid", resp.data.result);
                window.location.replace('/');
            }).catch(function(err) {
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

#beian {
    position: absolute;
    bottom: 0px;
    left: 0px;
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
