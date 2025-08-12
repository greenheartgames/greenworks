// Copyright (c) 2015 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

// Set steam_appid.txt to "480" (=Spacewar) on the root folder for these tests to work

var assert = require("assert");
var greenworks = require('../greenworks');

describe('greenworks API', function () {
  if (!greenworks.initAPI()) {
    console.log('An error occured initializing Steam API.');
    process.exit(1);
  }

  describe('saveTextToFile', function () {
    it('Should save successfully.', function (done) {
      greenworks.saveTextToFile('test_file.txt', 'test_content',
        function () { done(); }, function (err) { throw err; });
    });
  });

  describe('readTextFromFile', function () {
    it('Should read successfully.', function (done) {
      greenworks.readTextFromFile('test_file.txt', function (message) {
        assert(message == 'test_content'); done();
      }, function (err) {
        throw err;
      });
    });

    it('Should read failed.', function (done) {
      greenworks.readTextFromFile('not_exist.txt', function (message) {
        throw 'Error';
      }, function (err) { done(); });
    })
  });

  describe('enableCloud&isCloudEnabled', function () {
    it('', function () {
      greenworks.enableCloud(false);
      assert.equal(false, greenworks.isCloudEnabled());
      greenworks.enableCloud(true);
      assert.equal(true, greenworks.isCloudEnabled());
    });
  });

  describe('getCloudQuota', function () {
    it('Should get successfully', function (done) {
      greenworks.getCloudQuota(function (total, avail) { done(); },
        function (err) { throw err; });
    });
  });

  describe('activateAchievement', function () {
    it('Should get successfully', function (done) {
      greenworks.activateAchievement('ACH_WIN_ONE_GAME',
        function () { done(); },
        function (err) { throw err; });
    });
  });

  describe('Output Steam APIs calling result', function () {
    it('Should be called successfully', function (done) {
      console.log(greenworks.getCurrentGameLanguage());
      console.log(greenworks.getCurrentUILanguage());
      console.log(greenworks.getCurrentGameInstallDir());
      console.log(greenworks.getSteamId());
      console.log(greenworks.isSteamRunningOnSteamDeck());
      console.log(greenworks.getNumberOfPlayers(function (num) {
        console.log('Number of Players: ' + num);
        done();
      }, function (err) { throw err; }));
    });
  });
  
  describe('getAuthSessionTicketForWebAPI', function () {
    it('Should get successfully', function (done) {
      greenworks.getAuthSessionTicketForWebAPI('my_game', function (ticket) {
        assert(ticket.ticket);
        assert(ticket.ticket.length > 0);
        assert(ticket.handle);
        done();
      }, function (err) { throw err; });
    });
  });

  describe('getAuthSessionTicket', function () {
    const VALIDATE_AUTH_TICKET_EVENT = 'validate-auth-ticket';
    function registerValidateAuthTicketEvent(done, playerSteamId, expectedResult) {
      let callback = (steamId, eAuthSessionResponse, ownerSteamID) => {
        greenworks.removeListener(VALIDATE_AUTH_TICKET_EVENT, callback);

        assert.equal(steamId.getRawSteamID(), playerSteamId);
        assert.equal(eAuthSessionResponse, expectedResult);
        if (expectedResult === 0) {
          assert.equal(ownerSteamID.getRawSteamID(), playerSteamId);
        }

        done();
      };

      greenworks.on('validate-auth-ticket', callback);
    }

    it('Should get successfully - User auth', function (done) {
      greenworks.getAuthSessionTicket(function (ticket) {
        assert(ticket.ticket);
        assert(ticket.ticket.length > 0);
        assert(ticket.handle);

        const playerSteamId = greenworks.getSteamId().steamId;
        registerValidateAuthTicketEvent(done, playerSteamId, 0);

        const validateRes = greenworks.beginAuthSessionAsUser(ticket.ticket, playerSteamId);
        assert.equal(validateRes, 0); // k_EBeginAuthSessionResultOK  0 Ticket is valid for this game and this Steam ID.
      }, function (err) { throw err; });
    });

    it('Invalid SteamID - User auth', function (done) {
      greenworks.getAuthSessionTicket(function (ticket) {
        assert(ticket.ticket);
        assert(ticket.ticket.length > 0);
        assert(ticket.handle);
        const validateRes = greenworks.beginAuthSessionAsUser(ticket.ticket, '0');
        assert.equal(validateRes, 1); // k_EBeginAuthSessionResultInvalidTicket 1 The ticket is invalid.
        done();
      }, function (err) { throw err; });
    });

    it('Can cancel the ticket successfully', function (done) {
      greenworks.getAuthSessionTicket(async function (ticket) {
        greenworks.cancelAuthTicket(ticket.handle);

        await new Promise((resolve) => setTimeout(resolve, 2000)); // Wait for the ticket to be effectively cancelled

        const playerSteamId = greenworks.getSteamId().steamId;
        registerValidateAuthTicketEvent(done, playerSteamId, 6);

        const validateRes = greenworks.beginAuthSessionAsUser(ticket.ticket, playerSteamId);
        assert.equal(validateRes, 0); // k_EBeginAuthSessionResultOK  0 Ticket is valid for this game and this Steam ID.
      }, function (err) { throw err; });
    });

    it('Should get successfully - Server auth', function (done) {
      greenworks.getAuthSessionTicket(function (ticket) {
        assert(ticket.ticket);
        assert(ticket.ticket.length > 0);
        assert(ticket.handle);
        assert(greenworks.initGameServer(0x7f000001 /* 127.0.0.1 */, 51234, 51235, 3 /* authenticationAndSecure */, '1.0.0'));

        const playerSteamId = greenworks.getSteamId().steamId;
        registerValidateAuthTicketEvent(done, playerSteamId, 0);

        const validateRes = greenworks.beginAuthSessionAsServer(ticket.ticket, playerSteamId);
        assert.equal(validateRes, 0); // k_EBeginAuthSessionResultOK  0 Ticket is valid for this game and this Steam ID.
        done();
      }, function (err) { throw err; });
    });

    it('Invalid SteamID - Server auth', function (done) {
      greenworks.getAuthSessionTicket(function (ticket) {
        assert(ticket.ticket);
        assert(ticket.ticket.length > 0);
        assert(ticket.handle);
        assert(greenworks.initGameServer(0x7f000001 /* 127.0.0.1 */, 51244, 51245, 3 /* authenticationAndSecure */, '1.0.0'));
        const validateRes = greenworks.beginAuthSessionAsServer(ticket.ticket, '0');
        assert.equal(validateRes, 1); // k_EBeginAuthSessionResultInvalidTicket 1 The ticket is invalid.
        done();
      }, function (err) { throw err; });
    });
  });

  describe('getEncryptedAppTicket', function () {
    it('Should get successfully', function (done) {
      greenworks.getEncryptedAppTicket('test_content', function (ticket) {
        assert(ticket);
        done();
      }, function (err) { throw err; });
    });
  });

  describe('getFriends', function () {
    it('Should get successfully', function (done) {
      assert(greenworks.FriendFlags);
      assert(typeof greenworks.getFriendCount(greenworks.FriendFlags['All']), 'number');
      assert(greenworks.getFriends(greenworks.FriendFlags['All']));
      console.log(greenworks.getFriends(greenworks.FriendFlags['All']));
      done();
    });
  });
});
