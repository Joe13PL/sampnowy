import os
import sys
import glob
import pymysql
import time


def get_conn():
    host = os.getenv('TEST_DB_HOST', '127.0.0.1')
    port = int(os.getenv('TEST_DB_PORT', '3306'))
    user = os.getenv('TEST_DB_USER', 'root')
    password = os.getenv('TEST_DB_PASSWORD', '')
    db = os.getenv('TEST_DB_NAME', 'OpenRP_rp')
    # wait for db
    for i in range(20):
        try:
            conn = pymysql.connect(host=host, port=port, user=user, password=password, database=db, autocommit=True)
            return conn
        except Exception as e:
            print('DB not ready, retrying...', e)
            time.sleep(1)
    raise RuntimeError('Cannot connect to DB')


def run_migrations(conn):
    mig_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'rp_openmp', 'database', 'migrations')
    files = sorted(glob.glob(os.path.join(mig_dir, '*.sql')))
    for f in files:
        print('Applying', f)
        with open(f, 'r', encoding='utf-8') as fh:
            sql = fh.read()
        with conn.cursor() as cur:
            for stmt in [s.strip() for s in sql.split(';') if s.strip()]:
                cur.execute(stmt)


def test_groups(conn):
    with conn.cursor() as cur:
        name = 'test_group_{}'.format(int(time.time()))
        ranks = [f'r{i}' for i in range(10)]
        cur.execute("""
            INSERT INTO `groups` (`group_name`, `group_tag`, `group_type`, `group_color`, `group_bank`, `group_leader`, `group_max_members`, `group_flags`, `group_rank0`, `group_rank1`, `group_rank2`, `group_rank3`, `group_rank4`, `group_rank5`, `group_rank6`, `group_rank7`, `group_rank8`, `group_rank9`)
            VALUES (%s, 'T', 0, -1, 0, 0, 50, 0, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        """, (name, *ranks))
        cur.execute("SELECT `group_id` FROM `groups` WHERE `group_name` = %s", (name,))
        row = cur.fetchone()
        assert row, 'Inserted group not found'
        gid = row[0]
        cur.execute("SELECT `group_rank0`,`group_rank9` FROM `groups` WHERE group_id=%s", (gid,))
        r = cur.fetchone()
        assert r[0] == 'r0' and r[1] == 'r9'
        # cleanup
        cur.execute('DELETE FROM `groups` WHERE `group_id`=%s', (gid,))


def test_doors(conn):
    with conn.cursor() as cur:
        name = 'test_door_{}'.format(int(time.time()))
        cur.execute("""
            INSERT INTO `doors` (`door_name`, `door_type`, `door_owner_type`, `door_owner`, `door_locked`, `door_pickup`, `door_bank`, `door_ext_x`, `door_ext_y`, `door_ext_z`, `door_ext_a`, `door_ext_interior`, `door_ext_vw`, `door_int_x`, `door_int_y`, `door_int_z`, `door_int_a`, `door_int_interior`, `door_int_vw`)
            VALUES (%s, 0, 3, 0, 0, 1318, 0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0, 0)
        """, (name,))
        cur.execute('SELECT door_id, door_int_vw FROM `doors` WHERE `door_name`=%s', (name,))
        row = cur.fetchone()
        assert row, 'Inserted door not found'
        did = row[0]
        # check column added by migration exists
        cur.execute('SELECT `door_audio` FROM `doors` WHERE `door_id`=%s', (did,))
        a = cur.fetchone()
        assert a is not None
        cur.execute('DELETE FROM `doors` WHERE `door_id`=%s', (did,))


def main():
    conn = get_conn()
    try:
        run_migrations(conn)
        test_groups(conn)
        test_doors(conn)
        print('All integration tests passed')
    finally:
        conn.close()


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print('ERROR:', e)
        sys.exit(1)
    sys.exit(0)
