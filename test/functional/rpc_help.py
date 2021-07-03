#!/usr/bin/env python3
# Copyright (c) 2018 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test RPC help output."""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal, assert_raises_rpc_error

class HelpRpcTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1

    def run_test(self):
        node = self.nodes[0]

        # wrong argument count, note: Dash's help allows for two options since we utilize subcommands
        assert_raises_rpc_error(-1, 'help', node.help, 'foo', 'bar', 'foobar')

        # invalid argument
        assert_raises_rpc_error(-1, 'JSON value is not a string as expected', node.help, 0)

        # help of unknown command
        assert_equal(node.help('foo'), 'help: unknown command: foo')

        # command titles
        titles = [line[3:-3] for line in node.help().splitlines() if line.startswith('==')]
        assert_equal(titles, ['Addressindex', 'Blockchain', 'Control', 'Dash', 'Evo', 'Generating', 'Mining', 'Network', 'Rawtransactions', 'Util', 'Wallet', 'Zmq'])

if __name__ == '__main__':
    HelpRpcTest().main()