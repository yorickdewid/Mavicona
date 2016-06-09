/*
 * Copyright (C) 2005-2016 Christoph Rupp (chris@crupp.de).
 * All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the file COPYING for License information.
 */

using System;
using System.Collections.Generic;
using System.Text;
using Upscaledb;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Unittests
{
    public class CursorTest
    {
        private int counter;

        private int Callback(byte[] b1, byte[] b2) {
            counter++;
            if (b1.GetLength(0) < b2.GetLength(0))
                return (-1);
            if (b1.GetLength(0) > b2.GetLength(0))
                return (+1);
            for (int i = b1.GetLength(0); --i >= 0; ) {
                if (b1[i] < b2[i])
                    return (-1);
                if (b1[i] > b2[i])
                    return (+1);
            }
            return 0;
        }

        private Upscaledb.Environment env;
        private Database db;

        private void SetUp() {
            env = new Upscaledb.Environment();
            db = new Database();
            env.Create("ntest.db");
            db = env.CreateDatabase(1, UpsConst.UPS_ENABLE_DUPLICATE_KEYS);
        }

        private void TearDown() {
            env.Close();
        }

        private void Create() {
            Cursor c = new Cursor(db);
            c.Close();
        }

        private void Clone() {
            Cursor c1 = new Cursor(db);
            Cursor c2 = c1.Clone();
        }

        private void Move() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];

            k[0] = 0;
            db.Insert(k, r);
            k[0] = 1;
            db.Insert(k, r);
            k[0] = 2;
            db.Insert(k, r);
            k[0] = 3;
            db.Insert(k, r);
            k[0] = 4;
            db.Insert(k, r);

            c.Move(UpsConst.UPS_CURSOR_NEXT);
            c.Move(UpsConst.UPS_CURSOR_NEXT);
            c.Move(UpsConst.UPS_CURSOR_PREVIOUS);
            c.Move(UpsConst.UPS_CURSOR_LAST);
            c.Move(UpsConst.UPS_CURSOR_FIRST);
        }

        private void MoveNegative() {
            Cursor c = new Cursor(db);
            try {
                c.Move(UpsConst.UPS_CURSOR_NEXT);
            }
            catch (DatabaseException e) {
                Assert.AreEqual(UpsConst.UPS_KEY_NOT_FOUND, e.ErrorCode);
            }
        }

        private void MoveFirst() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MoveFirst();
        }

        private void MoveLast() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MoveLast();
        }

        private void MoveNext() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MoveNext();
        }

        private void MovePrevious() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MovePrevious();
        }

        void checkEqual(byte[] lhs, byte[] rhs)
        {
            Assert.AreEqual(lhs.Length, rhs.Length);
            for (int i = 0; i < lhs.Length; i++)
                Assert.AreEqual(lhs[i], rhs[i]);
        }
        
        private void TryMove()
        {
            Cursor c = new Cursor(db);
            byte[] k1 = BitConverter.GetBytes(1UL);
            byte[] r1 = BitConverter.GetBytes(2UL);
            db.Insert(k1, r1);
            byte[] k2 = null, r2 = null;
            Assert.IsTrue(c.TryMove(ref k2, ref r2, UpsConst.UPS_CURSOR_NEXT));
            checkEqual(k1, k2);
            checkEqual(r1, r2);
            Assert.IsFalse(c.TryMove(ref k2, ref r2, UpsConst.UPS_CURSOR_NEXT));
            Assert.IsNull(k2);
            Assert.IsNull(r2);
        }

        private void GetKey() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MovePrevious();
            byte[] f = c.GetKey();
            checkEqual(k, f);
        }

        private void GetRecord() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r = new byte[5];
            db.Insert(k, r);
            c.MovePrevious();
            byte[] f = c.GetRecord();
            checkEqual(r, f);
        }

        private void Overwrite() {
            Cursor c = new Cursor(db);
            byte[] k = new byte[5];
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            db.Insert(k, r1);
            c.MoveFirst();
            byte[] f = c.GetRecord();
            checkEqual(r1, f);
            c.Overwrite(r2);
            byte[] g = c.GetRecord();
            checkEqual(r2, g);
        }

        private void Find() {
            Cursor c = new Cursor(db);
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] k2 = new byte[5]; k2[0] = 6;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            db.Insert(k1, r1);
            db.Insert(k2, r2);
            c.Find(k1);
            byte[] f = c.GetRecord();
            checkEqual(r1, f);
            c.Find(k2);
            byte[] g = c.GetRecord();
            checkEqual(r2, g);
        }

        private void TryFind()
        {
            Cursor c = new Cursor(db);
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] k2 = new byte[5]; k2[0] = 6;
            byte[] k3 = new byte[5]; k3[0] = 7;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            db.Insert(k1, r1);
            db.Insert(k2, r2);
            var f = c.TryFind(k1);
            checkEqual(r1, f);
            var g = c.TryFind(k2);
            checkEqual(r2, g);
            var h = c.TryFind(k3);
            Assert.IsNull(h);
        }

        private void Insert() {
            Cursor c = new Cursor(db);
            byte[] q;
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] k2 = new byte[5]; k2[0] = 6;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            c.Insert(k1, r1);
            q = c.GetRecord();
            checkEqual(r1, q);
            q = c.GetKey();
            checkEqual(k1, q);

            c.Insert(k2, r2);
            q = c.GetRecord();
            checkEqual(r2, q);
            q = c.GetKey();
            checkEqual(k2, q);
        }

        private void InsertDuplicate() {
            Cursor c = new Cursor(db);
            byte[] q;
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            c.Insert(k1, r1);
            q = c.GetRecord();
            checkEqual(r1, q);
            q = c.GetKey();
            checkEqual(k1, q);

            c.Insert(k1, r2, UpsConst.UPS_DUPLICATE);
            q = c.GetRecord();
            checkEqual(r2, q);
            q = c.GetKey();
            checkEqual(k1, q);
        }

        private void InsertNegative() {
            Cursor c = new Cursor(db);
            byte[] q;
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            c.Insert(k1, r1);
            q = c.GetRecord();
            checkEqual(r1, q);
            q = c.GetKey();
            checkEqual(k1, q);

            try {
                c.Insert(k1, r2);
            }
            catch (DatabaseException e) {
                Assert.AreEqual(UpsConst.UPS_DUPLICATE_KEY, e.ErrorCode);
            }
        }

        private void Erase() {
            Cursor c = new Cursor(db);
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] r1 = new byte[5]; r1[0] = 1;
            c.Insert(k1, r1);
            c.Erase();
        }

        private void EraseNegative() {
            Cursor c = new Cursor(db);
            try {
                c.Erase();
            }
            catch (DatabaseException e) {
                Assert.AreEqual(UpsConst.UPS_CURSOR_IS_NIL, e.ErrorCode);
            }
        }

        private void GetDuplicateCount() {
            Cursor c = new Cursor(db);
            byte[] k1 = new byte[5]; k1[0] = 5;
            byte[] r1 = new byte[5]; r1[0] = 1;
            byte[] r2 = new byte[5]; r2[0] = 2;
            byte[] r3 = new byte[5]; r2[0] = 2;
            c.Insert(k1, r1);
            Assert.AreEqual(1, c.GetDuplicateCount());

            c.Insert(k1, r2, UpsConst.UPS_DUPLICATE);
            Assert.AreEqual(2, c.GetDuplicateCount());

            c.Insert(k1, r3, UpsConst.UPS_DUPLICATE);
            Assert.AreEqual(3, c.GetDuplicateCount());

            c.Erase();
            c.MoveFirst();
            Assert.AreEqual(2, c.GetDuplicateCount());
        }

        private void ApproxMatching()
        {
            Upscaledb.Environment env = new Upscaledb.Environment();
            Database db = new Database();
            byte[] k1 = new byte[5];
            byte[] r1 = new byte[5];
            k1[0] = 1; r1[0] = 1;
            byte[] k2 = new byte[5];
            byte[] r2 = new byte[5];
            k2[0] = 2; r2[0] = 2;
            byte[] k3 = new byte[5];
            byte[] r3 = new byte[5];
            k3[0] = 3; r3[0] = 3;
            try
            {
                env.Create("ntest.db");
                db = env.CreateDatabase(1);
                db.Insert(k1, r1);
                db.Insert(k2, r2);
                db.Insert(k3, r3);

                Cursor c = new Cursor(db);
                byte[] r = c.Find(k2, UpsConst.UPS_FIND_GT_MATCH);
                checkEqual(r, r3);
                checkEqual(k2, k3);
                k2[0] = 2;
                r = c.Find(k2, UpsConst.UPS_FIND_GT_MATCH);
                checkEqual(r, r1);
                checkEqual(k2, k1);
                db.Close();
                env.Close();
            }
            catch (DatabaseException e)
            {
                Assert.Fail("unexpected exception " + e);
            }
        }

        public void Run()
        {
            Console.WriteLine("CursorTest.Create");
            SetUp();
            Create();            
            TearDown();

            Console.WriteLine("CursorTest.Clone");
            SetUp();
            Clone();
            TearDown();

            Console.WriteLine("CursorTest.Move");
            SetUp();
            Move();
            TearDown();

            Console.WriteLine("CursorTest.MoveNegative");
            SetUp();
            MoveNegative();
            TearDown();

            Console.WriteLine("CursorTest.MoveFirst");
            SetUp();
            MoveFirst();
            TearDown();

            Console.WriteLine("CursorTest.MoveLast");
            SetUp();
            MoveLast();
            TearDown();

            Console.WriteLine("CursorTest.MoveNext");
            SetUp();
            MoveNext();
            TearDown();

            Console.WriteLine("CursorTest.MovePrevious");
            SetUp();
            MovePrevious();
            TearDown();

            Console.WriteLine("CursorTest.TryMove");
            SetUp();
            TryMove();
            TearDown();

            Console.WriteLine("CursorTest.GetKey");
            SetUp();
            GetKey();
            TearDown();

            Console.WriteLine("CursorTest.GetRecord");
            SetUp();
            GetRecord();
            TearDown();

            Console.WriteLine("CursorTest.Find");
            SetUp();
            Find();
            TearDown();

            Console.WriteLine("CursorTest.TryFind");
            SetUp();
            TryFind();
            TearDown();

            Console.WriteLine("CursorTest.Insert");
            SetUp();
            Insert();
            TearDown();

            Console.WriteLine("CursorTest.InsertDuplicate");
            SetUp();
            InsertDuplicate();
            TearDown();

            Console.WriteLine("CursorTest.InsertNegative");
            SetUp();
            InsertNegative();
            TearDown();

            Console.WriteLine("CursorTest.Erase");
            SetUp();
            Erase();
            TearDown();

            Console.WriteLine("CursorTest.EraseNegative");
            SetUp();
            EraseNegative();
            TearDown();

            Console.WriteLine("CursorTest.GetDuplicateCount");
            SetUp();
            GetDuplicateCount();
            TearDown();
        }
    }
}
